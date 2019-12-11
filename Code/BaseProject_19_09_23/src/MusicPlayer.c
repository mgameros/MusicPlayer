
#include "cmsis_os.h"  // CMSIS RTOS header file
#include "Board_LED.h"
#include "UART_driver.h"
#include "stdint.h"                     // data type definitions
#include "stdio.h"                      // file I/O functions
#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio.h"
#include "math.h"
#include "arm_math.h" // header for DSP library
#include <stdio.h>

// LED constants
#define LED_Green   0
#define LED_Orange  1
#define LED_Red     2
#define LED_Blue    3

#define NUM_CHAN	2 // number of audio channels
#define NUM_POINTS 1024 // number of points per channel
#define BUF_LEN NUM_CHAN*NUM_POINTS // length of the audio buffer
/* buffer used for audio play */
int16_t Audio_Buffer[BUF_LEN];
int16_t Audio_Buffer2[BUF_LEN];

#define ShowFiles_char "1"
#define PlaySong_char "2"
#define PauseSong_char "3"
#define StopSong_char "4"

// These are the commands for the system. They do the following:
// ShowFiles: This is the command from the GUI to refresh the list of files.
// SendComplete: This is the command from the FS thread that the list is refreshed.
// PlaySong: This is the command from the GUI to play a selected song.
// FindSong: This is the command to the FS thread to find the selected song.
// PauseSong: This is the command from the GUI to pause the currently playing song.
// StopSong: This is the command from the GUI to stop the currently playing/paused song.
enum commands {
	ShowFiles,
	SendComplete,
	PlaySong,
	FindSong,
	PauseSong,
	StopSong
};

// These are the states that the system can be in, all processed by the Process_Event function.
// NoState: What the system starts on. It doesn't do anything but change to the SongSelected state.
// SongSelected: A song is selected but nothing is playing.
// SongPlaying: A selected song is playing.
// SongPaused: There is a song in the queue that is currently paused.
// Refresh: The system is reading the USB file and getting the list of files.
enum state {
	NoState,
	SongSelected,
	SongPlaying,
	SongPaused,
	Refresh
};

// WAVE file header format
typedef struct WAVHEADER {
	unsigned char riff[4];						// RIFF string
	uint32_t overall_size;				// overall size of file in bytes
	unsigned char wave[4];						// WAVE string
	unsigned char fmt_chunk_marker[4];		// fmt string with trailing null char
	uint32_t length_of_fmt;					// length of the format data
	uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t channels;						// no.of channels
	uint32_t sample_rate;					// sampling rate (blocks per second)
	uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	uint16_t block_align;					// NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_chunk_header [4];		// DATA string or FLLR string
	uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} WAVHEADER;

// Global variables that need to be accessed by more than one function
char fileName[256];
bool wait;
bool stop;

void Control (void const *argument); // thread function
osThreadId tid_Control; // thread id
osThreadDef (Control, osPriorityNormal, 1, 0); // thread object

// Command queue from Rx_Command to Controller
osMessageQId mid_CMDQueue; // message queue for commands to Thread
osMessageQDef (CMDQueue, 1, uint32_t); // message queue object

osMessageQId mid_FSQueue; // message queue for commands to Thread
osMessageQDef (FSQueue, 1, uint32_t); // message queue object

#define MSGQUEUE_OBJECTS    1                 // number of Message Queue Objects

osMessageQId mid_MsgQueue;                     // message queue id
osMessageQDef (MsgQueue, MSGQUEUE_OBJECTS, int32_t);    // message queue object

// UART receive thread
void Rx_Command (void const *argument);  // thread function
osThreadId tid_RX_Command;  // thread id
osThreadDef (Rx_Command, osPriorityNormal, 1, 0); // thread object
         // define Thread_1

void FS_Thread(void const *arg);
osThreadId tid_FSThread;
osThreadDef (FS_Thread, osPriorityNormal, 1, 0);

osSemaphoreDef(SEM0);
osSemaphoreId(SEM0_id);

osSemaphoreDef(SEM1);
osSemaphoreId(SEM1_id);

void Process_Event (uint16_t event)
{
	osEvent evt;
	char fileName[256];
	static uint16_t		Current_State = NoState;
	switch(Current_State) {
		
		// This is only triggered when the application first turns on. 
		// It switches immediately to the first real state, SongSelected.
		case NoState:
			// Next State
			Current_State = SongSelected;
			// Exit actions
			// Transition actions
			// SongSelected entry actions
			break;
		
		// This is the state when a song is selected but hasn't been played yet.
		// It can either go to the refresh state if the "refresh" button is pressed
		// or it can go to a song playing if the "play" button is pressed.
		case SongSelected:
			// Switches to the SongPlaying state
			if(event == PlaySong)
			{
				// Next State
				Current_State = SongPlaying;
				// Exit actions
				// Transition actions
				// SongPlaying entry actions
			}
			// Switches to the Refreshing state
			else if(event == ShowFiles)
			{
				// Next State
				Current_State = Refresh;
				// Exit actions
				// Transition actions
				// ShowFiles entry actions
				osMessagePut(mid_FSQueue,ShowFiles,osWaitForever);
			}
			break;
		
		// This state is when the songs are being sent to the GUI.
		// It goes directly back to the SongSelected state because it just needs to go back to that.
		case SongPlaying:
			// Switches to the SongPaused state
			if(event == PauseSong)
			{
				// Next State
				Current_State = SongPaused;
				// Exit actions
				// Transition actions
				// SongPaused entry actions
			}
			// Switches to the SongSelected state
			else if(event == StopSong)
			{
				// Next State
				Current_State = SongSelected;
				// Exit actions
				// Transition actions
				// SongSelected entry actions
			}
			break;
		
		// This state is when there is a song playing. It can move to the SongPaused
		// state when the "pause" button is pressed or the StopSong state when the "stop"
		// button is pressed.
		case SongPaused:
			// Switches to the SongPlaying state
			if(event == PlaySong)
			{
				// Next State
				Current_State = SongPlaying;
				// Exit actions
				// Transition actions
				// SongPlaying entry actions
			}
			// Switches to the SongSelected state
			else if(event == StopSong)
			{
				// Next State
				Current_State = SongSelected;
				// Exit actions
				// Transition actions
				// SongSelected entry actions
			}
			break;
	
		// This state is when there is a song playing. It can move to the SongPlaying
		// state when the "play" button is pressed or the StopSong state when the "stop"
		// button is pressed.
		case Refresh:
			if(event == SendComplete)
			{
				// Next State
				Current_State = SongSelected;
				// Exit actions
				// Transition actions
				// SongSelected entry actions
			}
			break;
	}
}

// Initializes the threads, semaphore, and queues
void Init_Thread (void) {

	LED_Initialize(); // Initialize the LEDs
  UART_Init(); // Initialize the UART
  
	// Create queues
   mid_CMDQueue = osMessageCreate (osMessageQ(CMDQueue), NULL);  // create msg queue
  if (!mid_CMDQueue)return; // Message Queue object not created, handle failure
	mid_FSQueue = osMessageCreate (osMessageQ(FSQueue), NULL);  // create msg queue
  if (!mid_FSQueue)return; // Message Queue object not created, handle failure
  
	// Create threads
   tid_RX_Command = osThreadCreate (osThread(Rx_Command), NULL);
  if (!tid_RX_Command) return;
   tid_Control = osThreadCreate (osThread(Control), NULL);
  if (!tid_Control) return;
	tid_FSThread = osThreadCreate (osThread(FS_Thread),NULL);
	if(!tid_FSThread) return;
	mid_MsgQueue = osMessageCreate (osMessageQ(MsgQueue), NULL);  // create msg queue
  if (!mid_MsgQueue) {
   ; // Message Queue object not created, handle failure
  }
	
	// Create semaphore
	SEM0_id = osSemaphoreCreate(osSemaphore(SEM0),0);
	SEM1_id = osSemaphoreCreate(osSemaphore(SEM1),0);
}

// Controls the communication between the board and the UI
void Control (void const *argument)
{
	osEvent evt;
	Process_Event(0);
	
	while(1) 
	{
		evt = osMessageGet(mid_CMDQueue,osWaitForever); // receive command
		if(evt.status == osEventMessage)
		{
			Process_Event(evt.value.v);
		}
	}	
}

// Handles the communication between the board and the UI
void Rx_Command (void const *argument)
{
	char rx_char[2]={0,0};
	while(1) {
		UART_receive(rx_char,1);
		
		if(!strcmp(rx_char,ShowFiles_char)) 
		{
			osMessagePut(mid_CMDQueue,ShowFiles,osWaitForever);
		}
		if(!strcmp(rx_char,PlaySong_char)) 
		{
			osMessagePut(mid_CMDQueue,PlaySong,osWaitForever);
		}
		if(!strcmp(rx_char,PauseSong_char)) 
		{
			osMessagePut(mid_CMDQueue,PauseSong,osWaitForever);
		}
		if(!strcmp(rx_char,StopSong_char)) 
		{
			osMessagePut(mid_CMDQueue,StopSong,osWaitForever);
		}
	}
}

// Controls file reading fromt the flash drive
void FS_Thread(void const *arg)
{
	osEvent evt;
	fsFileInfo info;
	fsStatus status;
	usbStatus ustatus; // USB driver status variable
	uint8_t drivenum = 0; // Using U0: drive number
	char *drive_name = "U0:"; // USB drive name
	fsStatus fstatus; // file system status variable
	static FILE *f;
	char *StartFileList_msg = "5\n";
	char *EndFileList_msg = "6\n";
	
	ustatus = USBH_Initialize (drivenum); // initialize the USB Host
	if (ustatus == usbOK){
		// loop until the device is OK, may be delay from Initialize
		ustatus = USBH_Device_GetStatus (drivenum); // get the status of the USB device
		while(ustatus != usbOK){
			ustatus = USBH_Device_GetStatus (drivenum); // get the status of the USB device
		}
		// initialize the drive
		fstatus = finit (drive_name);
		if (fstatus != fsOK){
			// handle the error, finit didn't work
		} // end if
		// Mount the drive
		fstatus = fmount (drive_name);
		if (fstatus != fsOK){
			// handle the error, fmount didn't work
		} // end if 
		// file system and drive are good to go
		
		while(1) {
			info.fileID = 0;
			status = ffind("U0:*.*", &info);
		
			evt = osMessageGet(mid_FSQueue, osWaitForever);
			if(evt.status == osEventMessage) {
				if(evt.value.v == ShowFiles) {
				UART_send(StartFileList_msg,2); // Send start string
				while(status == fsOK)
				{
					status = ffind("U0:*",&info);			
					UART_send(info.name,strlen(info.name));
					UART_send("\n",1); // this is the VB string terminator "\n"	
				}

				UART_send(EndFileList_msg,2); // Send start string
				osMessagePut(mid_CMDQueue,SendComplete,osWaitForever);
				}
			}
		}
		
	} // end if USBH_Initialize
}