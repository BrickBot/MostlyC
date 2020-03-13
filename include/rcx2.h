#ifndef __RCX2__
#define __RCX2__
/**************************
 *      N O T E
 *  a C suffix on a name indicates all constants as arguments; otherwise, any expression
 *  a V suffix indicates that only a simple variable name is a legal argument
 **************************/
/**************************
 * sensors
 **************************/
#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2

// sensor types
#define SENSOR_TYPE_RAW 0
#define SENSOR_TYPE_SWITCH 1
#define SENSOR_TYPE_TEMPERATURE 2
#define SENSOR_TYPE_REFLECTION 3
#define SENSOR_TYPE_ROTATION 4

// sensor slopes
#define SENSOR_SLOPE_ABSOLUTE 0

// sensor modes
#define SENSOR_MODE_RAW 0
#define SENSOR_MODE_BOOLEAN 0x20
#define SENSOR_MODE_TRANSITION_COUNT 0x40
#define SENSOR_MODE_PERIOD_COUNTER 0x60
#define SENSOR_MODE_PCT_FULLSCALE 0x80
#define SENSOR_MODE_CELSIUS 0xa0
#define SENSOR_MODE_FAHRENHEIT 0xc0
#define SENSOR_MODE_ANGLE 0xe0

// resets the source 9 register, sensor value is recomputed; only useful for angle sensors
#define ClearSensorValueC(sensorNo) asm:C:C {"senz",(sensorNo)}
// Sets the sensor to the given type. Resets previous sensor state and sensor information.
#define SetSensorTypeC(sensorNo,sensorType) asm:C:C:C {"sent",(sensorNo),(sensorType)}
#define SetSensorType(sensorNo,sensorType) asm:C:C:C:I {"set"," 10",(sensorNo),(sensorType)}
// Sets the given mode and measurement slope(0 absolute, 1-31 dynamic) for the given sensor, also resets the sensor value
#define SetSensorModeC(sensorNo,sensorMode,slope) asm:C:C:C:C {"senm",(sensorNo),(sensorMode),(slope)}
#define SetSensorMode(sensorNo,sensorMode) asm:C:C:C:I {"set"," 11",(sensorNo),(sensorMode)}

/**************************
 * motors
 **************************/
// constants for selecting output "motorList"

#define MOTOR_A		0x01
#define MOTOR_B		0x02
#define MOTOR_C		0x04
#define MOTOR_AB	0x03
#define MOTOR_AC	0x05
#define MOTOR_BC	0x06
#define MOTOR_ABC	0x07

// output modes
#define MOTOR_MODE_FLOAT	0
#define MOTOR_MODE_OFF		1
#define MOTOR_MODE_ON		2

// directions
#define MOTOR_BACKWARDS 0
#define MOTOR_REVERSE 1
#define MOTOR_FORWARDS 2

#define OnC(motorList)		asm:C:C:C { "out", MOTOR_MODE_ON,(motorList) } 
#define OffC(motorList)		asm:C:C:C { "out", MOTOR_MODE_OFF,(motorList) }
#define FloatC(motorList)	asm:C:C:C { "out", MOTOR_MODE_FLOAT,(motorList) }
#define BackC(motorList)	asm:C:C:C { "dir", MOTOR_BACKWARDS,(motorList) };asm:C:C:C { "out", MOTOR_MODE_ON,(motorList) }
#define ForwardC(motorList)	asm:C:C:C { "dir", MOTOR_FORWARDS,(motorList) };asm:C:C:C { "out", MOTOR_MODE_ON,(motorList) }
#define ReverseC(motorList)	asm:C:C:C { "dir", MOTOR_REVERSE,(motorList) };asm:C:C:C { "out", MOTOR_MODE_ON,(motorList) }
#define GoC(motorList)		asm:C:C:C { "dir", MOTOR_FORWARDS,(motorList) };asm:C:C:C { "out", MOTOR_MODE_ON,(motorList) }
#define StopC(motorList)	asm:C:C:C { "out", MOTOR_MODE_OFF,(motorList) }
#define CoastC(motorList)	asm:C:C:C { "out", MOTOR_MODE_FLOAT,(motorList) }
// sets the Motor Status register
#define OnOffFloatC(motorList,mode) 	asm:C:C:C { "out",(mode),(motorList) }
// sets the Motor Status register
#define SetDirectionC(motorList,direction) asm:C:C:C {"dir",(direction),(motorList)}
// sets the Global Motor Status register
#define GlobalOnOffFloatC(motorList,mode) asm:C:C:C {"gout",(mode),(motorList)}
// sets the Global Motor Status register
#define GlobalDirectionC(motorList,direction) asm:C:C:C {"gdir",(direction),(motorList)}
// sets the Motor Status registers with the new power levels (0-7)
#define SetPower(motorList,power) asm:C:C:I {"pwr",(motorList),(power)}
// sets the Global Motor Status registers with the new power levels (0-7)
#define SetMaxPower(motorList,power) asm:C:C:I {"gpwr",(motorList),(power)}

/**************************
 * sound
 **************************/
// sounds - for PlaySystemSound()

#define SOUND_CLICK		0
#define SOUND_DOUBLE_BEEP	1
#define SOUND_DOWN		2
#define SOUND_UP		3
#define SOUND_ERROR		4
#define SOUND_FAST_UP		5

// empties the sound buffer in the RCX from any and all queued tones or system sounds
#define ClearSound() asm:C {"playz"}
// empties the sound buffer and ignores future sounds
#define MuteSound() asm:C {"mute"}
// restarts the processing of sounds
#define UnmuteSound() asm:C {"speak"}
// plays the given system sound, if sounds are globally enabled.
#define PlaySystemSoundC(sound) asm:C:C {"plays", (sound)}
// variable is the frequency, puts the tone and duration in the RCX sound buffer
#define PlayTone(frequency,centiseconds) asm:C:V:C {"playv",(frequency),(centiseconds)}
// queues the tone in the RCX sound buffer, if enabled
#define PlayToneC(frequency,centiseconds) asm:C:C:C {"playt",(frequency),(centiseconds)}

/**************************
 * display
 **************************/
	// display modes - for SelectDisplay

#define DISPLAY_WATCH		0
#define DISPLAY_SENSOR_1	1
#define DISPLAY_SENSOR_2	2
#define DISPLAY_SENSOR_3	3
/* don't work in firmware 0328
#define DISPLAY_OUT_A		4
#define DISPLAY_OUT_B		5
#define DISPLAY_OUT_C		6
#define DISPLAY_USER	    7
*/
/* uses the value in the variable to select the display */
#define SelectDisplay(source) asm:C:I {"view",(source)}
/* changes display as a global variable changes, precision sets the decimal point */
#define ViewSourceValue(precision,variable) asm:C:C:G {"disp",(precision),(variable)}
/* changes display as a global variable changes */
#define Show(x) ViewSourceValue(0,x)

/**************************
 * communication
 **************************/
#define IR_POWER_SHORT 0
#define IR_POWER_LONG 1

#define UART_PACKCONF_PREAMBLE 1
#define UART_PACKCONF_NEGATED_CHECKSUM 2
#define UART_PACKCONF_CHECKSUM 4

#define UART_TRANSMIT_2400_BAUD 0
#define UART_TRANSMIT_4800_BAUD 1

#define UART_TRANSMIT_CARRIER_38_KHZ 0
#define UART_TRANSMIT_CARRIER_76_KHZ 2

#define UART_TRANSMIT_DUTYCYCLE_50_PERCENT 0
#define UART_TRANSMIT_DUTYCYCLE_25_PERCENT 4

// resets the IR message buffer to zero
#define ClearPBMessage() asm:C {"msgz"}
// sets the IR transmission range in the RCX
#define PBTXPowerC(power) asm:C:C {"txs", ((power)&1)}
// sets the IR message bufferand then checks for possible events
#define InternMessageC(byte) asm:C:C {"msgs",(byte)}
// sends the low 8 bits of that value as an IR message (InternMessage,0xF7)
#define SendPBMessage(source) asm:C:I {"msg",(source)}
// reads the communication setup from the UART source (33) and then sends data from the same buffer
// 0<=start<=15  1<=size<=(16-start)
#define SendUARTDataC(start,size) asm:C:C:C {"uart",(start),(size)}
#define SetUARTBufferC(byte0to15,value) asm:C:C:C:I {"set"," 33",(byte0to15),(value)}
#define SetUARTPackConf(packconf) asm:C:C:C:I {"set"," 33"," 16",(packconf)}
#define SetUARTTransmitConf(transmitconf) asm:C:C:C:I {"set"," 33"," 17",(transmitconf)}

/**************************
 * timers
 * 15-bit unsigned (actually 16-bit signed with 0x7FFF-0 wrap-around) with 100 ms resolution.
 * Writing to them also affects the high-resolution timers and vice-versa.
 **************************/
#define TIMER_POWERDOWN_NEVER 0

// resets the power-down timer
#define PBAliveOrNot() asm:C {"ping"}
// resets the given system timer to zero.
#define ClearTimerC(timerNo) asm:C:C {"tmrz",(timerNo)}
// sets the power down time to the given amount and resets the power down timer
#define PBPowerDownTimeC(minutes) asm:C:C {"tout",(minutes)}
// sets the internal watch to the given time
#define SetWatchC(hours,minutes) asm:C:C:C {"setw",(hours),(minutes)}
//The source is interpreted as “hours*60 + minutes” and cannot exceed 23 hrs and 59 min.
#define SetWatch(hours,minutes) asm:C:C:C:I {"set"," 14"," 0",((hours)*60+(minutes))}
#define SetTimer0(value) asm:C:C:C:I {"set"," 1"," 0",(value)}
#define SetTimer1(value) asm:C:C:C:I {"set"," 1"," 1",(value)}
#define SetTimer2(value) asm:C:C:C:I {"set"," 1"," 2",(value)}
#define SetTimer3(value) asm:C:C:C:I {"set"," 1"," 3",(value)}
#define Set10msTimer0(value) asm:C:C:C:I {"set","26"," 0",(value)}
#define Set10msTimer1(value) asm:C:C:C:I {"set","26"," 1",(value)}
#define Set10msTimer2(value) asm:C:C:C:I {"set","26"," 2",(value)}
#define Set10msTimer3(value) asm:C:C:C:I {"set","26"," 3",(value)}

/**************************
 * counters
 **************************/

/**************************
 * events
 **************************/
#define EVENT_STATE_LOW 0
#define EVENT_STATE_NORMAL 1
#define EVENT_STATE_HIGH 2
#define EVENT_STATE_UNDEFINED 3
#define EVENT_STATE_CALIBRATION_START 4
#define EVENT STATE_CALIBRATING 5

#define EVENT_MODE_STATE "25"
#define EVENT_MODE_CLICK "27"
#define EVENT_MODE_UPPER "28"
#define EVENT_MODE_LOWER "29"
#define EVENT_MODE_HYSTERESIS "30"
#define EVENT_MODE_DURATION "31"

// clears all the 16 events. Tasks that actively waiting/monitoring are not alerted
#define ClearAllEvents() asm:C {"dele"}
#define SetEvent(Mode,EventNumber0to15,Value) asm:C:C:C:I {"set",(Mode),(EventNumber0to15),(Value)}

//	E V E N T   N U M B E R S
#define EVENT_PRESSED 0
#define EVENT_RELEASED 1
#define EVENT_PERIOD 2
#define EVENT_TRANSITION 3
#define EVENT_CHANGERATE_EXCEEDED 7
#define EVENT_ENTER_LOW 8
#define EVENT_ENTER_NORMAL 9
#define EVENT_ENTER_HIGH 10
#define EVENT_CLICK 11
#define EVENT_DOUBLECLICK 12
#define EVENT_MAILBOX 14

//	E V E N T   B I T S
#define EVENT_PRESSED_BIT (1<<0)
#define EVENT_RELEASED_BIT (1<<1)
#define EVENT_PERIOD_BIT (1<<2)
#define EVENT_TRANSITION_BIT (1<<3)
#define EVENT_CHANGERATE_EXCEEDED_BIT (1<<7)
#define EVENT_ENTER_LOW_BIT (1<<8)
#define EVENT_ENTER_NORMAL_BIT (1<<9)
#define EVENT_ENTER_HIGH_BIT (1<<10)
#define EVENT_CLICK_BIT (1<<11)
#define EVENT_DOUBLECLICK_BIT (1<<12)
#define EVENT_MAILBOX_BIT (1<<14)

#define GetEventState(eventno) event(0,eventno)
#define GetEventClicks(eventno) event(1,eventno)
#define GetEventUpperThreshold(eventno) event(2,eventno)
#define GetEventLowerThreshold(eventno) event(3,eventno)
#define GetEventHysteresis(eventno) event(4,eventno)
#define GetEventDuration(eventno) event(5,eventno)
// Forces the firmware to behave as if the events, whose bits are set in the calculated 16 bit value, had actually happened.
#define DirectEventsC(eventbits) asm:C:I {"event",(eventbits)}
// Sets the thresholds and hysteresis for the specified event number.
// The program must be halted for some time, in order to allow the firmware to do the calibration.
#define CalibrateEventC(eventno,upper,lower,hysteresis) asm:C:C:C:C:C {"cale",(eventno),(upper),(lower),(hysteresis)}

/**************************
 * data log
 **************************/
// clears the current data log buffer and allocates a new one
#define SetDataLogC(size) asm:C:C {"logz",(size)}
// only variables, timers, sensors and the watch (sources 0, 1, 9 and 14) can be logged
// Use Sensor1 to Sensor3, Timer0 to Timer3, Watch
#define DataLog(source) asm:C:H {"log",(source)}

/**************************
 * resource access
 **************************/
#define ACCESS_MOTOR_A 1
#define ACCESS_MOTOR_B 2
#define ACCESS_MOTOR_C 8
#define ACCESS_SOUND   4

//labels are referenced as "W1" "W2" etc. and defined as "W1:" "W2:"

// When the event occurs, execution continues at the given address and the monitoring is suspended.
// Afterwards the actual event(s) can be read from source 23 if the task needs to distinguish between several different causes.
#define EnterEventCheckC(eventbits,label) asm:C:I:C {"monel",(eventbits),label}
// stops event monitoring for the task
#define ExitEventCheck() asm:C {"monex"}

// Tries to gain access to the requested resources (OR them together to request more than one)
// with the tasks priority. If the task succeeds, execution continues. If the task fails to acquire the
// resources or if the resources are later claimed by another task with a higher priority (before
// releasing the resources), execution continues from the label given in the command.
#define EnterAccessCheckC(resources,label) asm:C:C:C {"monal",(resources),label}
// exits the access control region if the task were inside one
#define ExitAccessCheck() asm:C {"monax"}

#define DefineLabelC(label) asm:C {label}

/**************************
 * brick status
 **************************/

/**************************
 * general
 **************************/
/* Maximum value that can be returned by the rand function. */
#define RAND_MAX 0x7fff
/* Minimum and maximum macros */
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define srand(seed) asm:C:C:C:I {"set"," 4"," 0",(seed)}

/**************************
 * execution control
 **************************/
// turns the RCX off by resetting the power-down timer to zero
#define PBTurnOff() asm:C {"offp"}
// stops all (running) tasks in the current program, releases acquired resources
#define StopAllTasks() asm:C {"stop"}
// (re)starts the given task if the task slot is non-empty
#define StartTaskC(taskNameOrNum) asm:C:T {"start",(taskNameOrNum)}
// stops all tasks in the current program, task 0 is started in the new program in slot 0-4
#define SetProgramSlotC(programNo) asm:C:C {"prgm", (programNo)}
#define SetProgramSlot(programNo) asm:C:C:C:I {"set"," 8"," 0",(programNo)}
// sets the priority level, 256 levels can be used, with 0 having highest priority
#define SetPriorityC(priority) asm:C:C {"setp",(priority)}
// Suspends the task execution for the given number of 10 ms, if number is negative, the command is ignored.
#define Wait(centiseconds) asm:C:I {"wait",(centiseconds)}

#endif //__RCX2__
