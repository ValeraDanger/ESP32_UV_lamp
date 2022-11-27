Refactoring and freertos-using branch of code

=============================================

  

IMPORTANT: changed interaction protocol. Now you command is
`<controlled object>`:`<action>`:`<optional>`#.
(Only relay control is able now. )


Where:

-`<controlled object>` : object with you want act to. Only "relay" or "timer" at this moment

-`<action>` : action, the object should do. For relay it's "on" or "of" 

-`<optional>` : other arguments for diffrent object.

Command examples: "`relay`:`on`\#" "`relay`:`off`\#"
