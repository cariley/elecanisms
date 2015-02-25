
import usb.core

class myUsb:

    def __init__(self):
        self.HELLO = 0
        self.SET_VALS = 1
        self.GET_VALS = 2
        self.PRINT_VALS = 3

        # what can we transfer?
        self.SPRING = 1
        self.DAMPER = 2
        self.WALL = 3
        self.TEXTURE = 4

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def hello(self):
        try:
            self.dev.ctrl_transfer(0x40, self.HELLO)
        except usb.core.USBError:
            print "Could not send HELLO vendor request."

    def set_vals(self, val1, val2):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_VALS, int(val1), int(val2))
        except usb.core.USBError:
            print "Could not send SET_VALS vendor request."

    def get_vals(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_VALS, 0, 0, 4)
        except usb.core.USBError:
            print "Could not send GET_VALS vendor request."
        else:
            return [int(ret[0])+int(ret[1])*256]

    def print_vals(self):
        try:
            self.dev.ctrl_transfer(0x40, self.PRINT_VALS)
        except usb.core.USBError:
            print "Could not send PRINT_VALS vendor request."


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

notDone = True
while(notDone):
    comlink = myUsb()
    myInput = raw_input("Send command: ")

    # check for valid inputs
    myInput = myInput.strip().lower().split(',')
    # print("Debugging, text is: " + str(myInput))
    text = myInput[0]

    command = -1
    argument = -1
    if len(myInput) == 1: # no number included  
        if text == 'help':
            print("Valid commands: ")
            print("spring, spring coefficient (float)")
            print("damper, damper coefficient (float)")
            print("texture")
            print("wall")
            print("'exit' or 'quit' to leave program.")
        elif text == 'texture':
            command = comlink.TEXTURE
        elif text == 'wall':
            command = comlink.WALL
        elif text == 'quit' or text == 'exit':
            notDone = False
        else: # no valid command found
            print("Command not valid, or is missing a coefficient argument. Try typing help to see valid commands.")
    elif len(myInput) == 2 and is_number(myInput[1]): # , two numbers included,
    # a command and an argument
        argument = float(myInput[1])
        if text == 'spring':
            command = comlink.SPRING
        elif text == 'damper':
            command = comlink.DAMPER
        else:
            # invalid command
            print("Command not valid, or had an unexpected coefficient argument. Try typing help to see valid commands.")
    elif len(myInput) == 2:
        #second argument isn't a number
        print("Coefficient value is not a number. Try typing help to see valid commands.")
    else:
        # wrong number of arguments
        print("Wrong number of arguments. Try typing help to see valid commands.")

    if command != -1: # valid command found
        comlink.set_vals(command, argument)
        print("Command sent!")





