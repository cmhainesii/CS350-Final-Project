# CS350 - Southern New Hampshire University - Portfolio

# Thermostat Project

# Summarize the project and what problem it was solving

This project was to use the TI developement board to create a thromostat. A LED attached to the board turns on to simulate a heater turning on when the thermostat detects that the current temperature is lower than the temperature the thermostat is currently set for. The temperature can be increased and decreased using buttons attached to the development board. The program outputs information on the current temperature, thermostat setting temperature, if the heater is on or off, and the number of seconds since the program started running via serial console using UART. This project makes use of many of the development board's peripherals.

# What did you do particularly well?

I managed to keep the code pretty simple for this project since there isn't much logic required in determining when to turn the heat on and off. Since many drivers were used to communicate with the various peripherals, keeping the code organized made the project more manageable.

# What could you improve?

I don't think I understood how to properly implement the task scheduler. While my implementation works, I think I could implement a modular task scheduler which could then be repurposed for use in other projects. I may continue to improfve my solution and implement a proper task scheduler as I continue to practice working with embedded systems.

# What tools and/or resources are you adding to your support network?

I enjoyed working with embedded systems so much that I purchased a kit of compoenents so I can continue learning and writing more code to power these new components! The documentation for the development board and software development kit was useful in helping me figure out how to use the various drivers needed to operate the development board's periphrals.

# What skills from this project will be particularly transferable to other projects and/or course work?

Learning how to code for embedded systems emphisized the importance of writing code that is clean and efficient. I can use these skills when writing programs for any platform in order to minimize resource usage and write programs that are faster, more efficient, and easier to read and maintain.

# How did you make this project maintainable, readable, and adaptable?

I used various programming constructs to keep my code clean and organized. I always use comments to explain what various parts of the code are doing so that someone reading the code can easily follow along with what is happening. Grouping similar variables into structs with clearly defined names is one way that I kept my code maintainable, readable, and adaptable.e

# Morse Code Project

# Summarize the project and what problem it was solving

This project was all about working with serial input and output. The program starts by flashing morse code via one of the integrated LEDs that spelled out 'SOS' (international morse code). I made use of interrupts to detect when a button was pressed. After detecting a button has been pressed, the message changes from 'SOS' to 'OK', but only after the current message has finished transmitting.

# What did you do particularly well?

I realized after I got the code written and working as intended that the code was messier than it needed to be. For instance, 'SOS' was designed with three states for each letter when I was planning the program and designing the state machine diagram. I realized that I could have just made it two states since 'S' appears twice, but I left it how it was. What I realized that I could do was use fall through in my switch/case block for the state actions so I only had to write the code for flashing an 'S' in morse code once. I wanted to have three states so I could easily keep track of which character I was currently transmitting. So using the switch/case fallthrough was a perfect way to allow me to keep three states without duplicating code.

# What could you improve?

I think the overall design could still be further simplified. The program and process works as intended but I still think I could clean up the design of the state machine.

# What tools and/or resources are you adding to your support network?

Discussing the project with others who are knowledgable about development and/or embedded systems is a valueble resource that I could add to my support network.

# What skills from this project will be particularly transferable to other projects and/or course work?

I think the practice I got when coming up with the design was invaluable. It's tough to go from the idea you have in your head to actually creating a plan of how to do it in black and white. This project really made me think about how I was going to design it in a way that worked and made sense.

# How did you make this project maintainable, readable, and adaptable?

I always use comments to explain anything that isn't immediately obvious to someone reading the code. Using functions and other programming constructs allowed me to keep my code organized and modular.

