# Refactoring and freertos-using branch of code

IMPORTANT: changed interaction protocol. Now you command is
 `<controlled object>:<action>:<other>#`.



Where:

-`<controlled object>` : object with you want act to. Only `relay` or `timer` at this moment

-`<action>` : action, the object should do. 

 `<action>` for `relay` object:
 - `on` : set relay on
 - `off` : set relay off
 
 `<action>` for `timer` object:
 - `settimer` : set relay on for some time

-`<other>` : other arguments for diffrent object.
 
`<other>` for `timer` object:	 

 -  `<time>` : int number, time, you want to run timer, seconds



Command examples:
 

    relay:on#
    relay:off#
    timer:settimer:5#
