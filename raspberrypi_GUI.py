import serial
import time
import tkinter as tk

# Define buttons
def serialSend(a):
    # ser.write(bytes(a,'UTF-8'))
    print("Send: "+str(a))

def start():
    global inputType
    global status
    global input_pressure
    global input_flowrate
    # Change status
    status.set("Pressurizing")
    display_status.config(fg="black",bg="white")

    # Send starting sign, inputType, and input to Arduino
    serialSend("S")
    if (inputType.get()):
        # Selected Flowrate
        serialSend(inputType.get())
        serialSend(input_flowrate.get())
    else:
        # Selected Pressure
        serialSend(inputType.get())
        serialSend(input_pressure.get())

# # Connect serial with Arduino
# ser = serial.Serial('com7', 9600)
# print("Reset Arduino")
# time.sleep(3)

# Make GUI
tkTop = tk.Tk()
tkTop.title("Pump-on-a-Chip v.1.1")

# Global variables
inputType = tk.IntVar()
status = tk.StringVar()
status.set("Ready")

# Status
display_status = tk.Label(tkTop,
    textvariable=status,
    font=("Arial", 20),
    padx=30,
    pady=5,
    relief="solid",
    fg="black",
    bg="green",
    width=20,
    height=1
)
display_status.grid(row=0,column=0,columnspan=6)

# Initial input - Pressure
tk.Radiobutton(tkTop,
    text="Pressure: ",
    font=("Arial", 10),
    variable=inputType,
    value=0
).grid(row=1,column=0)
input_pressure = tk.Entry(tkTop,
    width=15,
    font=("Arial", 10),
    borderwidth=5
)
input_pressure.grid(row=1,column=1)
tk.Label(tkTop,
    text="mbar",
    font=("Arial", 10)
).grid(row=1,column=2)

# Initial input - flowrate
tk.Radiobutton(tkTop,
    text="Flowrate: ",
    font=("Arial", 10),
    # padx=5,
    # pady=5,
    variable=inputType,
    value=1
).grid(row=2,column=0)
input_flowrate = tk.Entry(tkTop,
    width=15,
    font=("Arial", 10),
    borderwidth=5
)
input_flowrate.grid(row=2,column=1)
tk.Label(tkTop,
    text="ml/min",
    font=("Arial", 10)
).grid(row=2,column=2)

# Start button
tk.Button(tkTop,
    text="START",
    command=start,
    height=4,
    width=15
).grid(row=1,column=3,rowspan=2)


# varLabel = tk.StringVar()
# tkLabel = tk.Label(textvariable=varLabel, )
# tkLabel.pack()

# varLabel2 = tk.IntVar()
# tkLabel2 = tk.Label(textvariable=varLabel2, )
# tkLabel2.pack()

# button1 = tk.IntVar()
# button1state = tk.Button(tkTop,
#     text="ON",
#     command=set_button1_state,
#     height = 4,
#     fg = "black",
#     width = 8,
#     bd = 5,
#     activebackground='green'
# )
# button1state.pack(side='top', ipadx=10, padx=10, pady=15)

# button2 = tk.IntVar()
# button2state = tk.Button(tkTop,
#     text="OFF",
#     command=set_button2_state,
#     height = 4,
#     fg = "black",
#     width = 8,
#     bd = 5
# )
# button2state.pack(side='top', ipadx=10, padx=10, pady=15)

# tkButtonQuit = tk.Button(
#     tkTop,
#     text="Quit",
#     command=quit,
#     height = 4,
#     fg = "black",
#     width = 8,
#     bg = 'yellow',
#     bd = 5
# )
# tkButtonQuit.pack(side='top', ipadx=10, padx=10, pady=15)

tk.mainloop()