# Refactoring and freertos-using branch of code

IMPORTANT: changed interaction protocol. Now you command is
 `<controlled object>:<action>:<other>#`.



Where:

-`<controlled object>` : object with you want act to. Only `relay` or `timer` at this moment

-`<action>` : action, the object should do. 

 `<action>` for `relay` object:
 - `on` : set relay on
 - `off` : set relay off
 - `status` : returns relay status code
 
 `<action>` for `timer` object:
 - `settimer` : set relay on for some time
 - `pause` : pause timer until start
 - `start` : start paused timer
 - `status` : returns timer status (3 or 4, and 5 if paused)
 - `gettime` : returns timer time left in ms

-`<other>` : other arguments for diffrent object.
 
`<other>` for `timer` object:	 

 -  `<time>` : int number, time, you want to run timer, seconds



Command examples:
 

    relay:on#
    relay:off#
    timer:settimer:5#
