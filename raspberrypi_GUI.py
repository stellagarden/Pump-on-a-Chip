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
    status.set("Pressure Reservoir Checking")
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

def p_load_cells(event):
    serialSend("C")

def r_load_cells(event):
    serialSend("CR")

def p_run_pump(event):
    serialSend("P")

def r_run_pump(event):
    serialSend("PR")

def p_vent_p_res(event):
    serialSend("A")

def r_vent_p_res(event):
    serialSend("AR")

def p_vent_c_res(event):
    serialSend("B")

def r_vent_c_res(event):
    serialSend("BR")

def set_prop_valve(event):
    serialSend("V")
    serialSend(input_prop.get())


# # Connect serial with Arduino
# ser = serial.Serial('com7', 9600)
# print("Reset Arduino")
# time.sleep(3)

# Make GUI
tkTop = tk.Tk()
tkTop.title("Pump-on-a-Chip   v.1.1")

# Global variables
inputType = tk.IntVar()
status = tk.StringVar()
status.set("Ready")
resP = tk.DoubleVar()
cellP = tk.DoubleVar()
flowrate = tk.DoubleVar()
prop_valve = tk.IntVar()
print_prop_valve = tk.StringVar()
input_prop = tk.IntVar()

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
).grid(row=1,column=0,columnspan=2)
input_pressure = tk.Entry(tkTop,
    width=15,
    font=("Arial", 10),
    borderwidth=5
)
input_pressure.grid(row=1,column=2)
tk.Label(tkTop,
    text="mbar",
    font=("Arial", 10)
).grid(row=1,column=3)

# Initial input - flowrate
tk.Radiobutton(tkTop,
    text="Flowrate: ",
    font=("Arial", 10),
    variable=inputType,
    value=1
).grid(row=2,column=0,columnspan=2)
input_flowrate = tk.Entry(tkTop,
    width=15,
    font=("Arial", 10),
    borderwidth=5
)
input_flowrate.grid(row=2,column=2)
tk.Label(tkTop,
    text="ml/min",
    font=("Arial", 10)
).grid(row=2,column=3)

# Start button
tk.Button(tkTop,
    text="START",
    command=start,
    padx=30,
    pady=20
).grid(row=1,column=4,rowspan=2,columnspan=2)

# Display sensor values
tk.Label(tkTop,
    text="Pressure\nReservoir",
    font=("Arial", 10),
    pady=5
).grid(row=3,column=0,columnspan=2)
tk.Label(tkTop,
    textvariable=resP,
    font=("Arial", 10),
    pady=5
).grid(row=4,column=0,columnspan=2)

tk.Label(tkTop,
    text="Cell\nReservoir",
    font=("Arial", 10)
).grid(row=3,column=2,columnspan=2)
tk.Label(tkTop,
    textvariable=cellP,
    font=("Arial", 10)
).grid(row=4,column=2,columnspan=2)

tk.Label(tkTop,
    text="Flowrate",
    font=("Arial", 10)
).grid(row=3,column=4,columnspan=2)
tk.Label(tkTop,
    textvariable=flowrate,
    font=("Arial", 10)
).grid(row=4,column=4,columnspan=2)

# Administrator mode - manual control buttons
b_load_cells = tk.Button(tkTop,
    text="Load\nCells",
    width=10,
    height=3
)
b_load_cells.grid(row=5,column=0,rowspan=3)
b_load_cells.bind("<Button-1>", p_load_cells)
b_load_cells.bind("<ButtonRelease-1>", r_load_cells)

b_run_pump = tk.Button(tkTop,
    text="Run\nPump",
    width=10,
    height=3
)
b_run_pump.grid(row=5,column=1,rowspan=3)
b_run_pump.bind("<Button-1>", p_run_pump)
b_run_pump.bind("<ButtonRelease-1>", r_run_pump)

b_vent_p_res = tk.Button(tkTop,
    text="Vent\nPressure\nReservoir",
    width=10,
    height=3
)
b_vent_p_res.grid(row=5,column=2,rowspan=3)
b_vent_p_res.bind("<Button-1>", p_vent_p_res)
b_vent_p_res.bind("<ButtonRelease-1>", r_vent_p_res)

b_vent_c_res = tk.Button(tkTop,
    text="Vent\nCell\nReservoir",
    width=10,
    height=3
)
b_vent_c_res.grid(row=5,column=3,rowspan=3)
b_vent_c_res.bind("<Button-1>", p_vent_c_res)
b_vent_c_res.bind("<ButtonRelease-1>", r_vent_c_res)

tk.Label(tkTop,
    text="Proportional Valve",
    font=("Arial", 10)
).grid(row=5,column=4)
print_prop_valve.set(str(prop_valve.get())+' %')
tk.Label(tkTop,
    textvariable=print_prop_valve,
    font=("Arial", 10)
).grid(row=6,column=4)

b_vent_c_res = tk.Button(tkTop,
    text="Run\nPump",
    width=10,
    height=3
)
b_load_cells.grid(row=5,column=1,rowspan=2)
b_load_cells.bind("<Button-1>", p_run_pump)
b_load_cells.bind("<ButtonRelease-1>", r_run_pump)


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