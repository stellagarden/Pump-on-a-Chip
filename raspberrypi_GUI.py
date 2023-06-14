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
    if (inputType.get()):
        # Selected Flowrate
        serialSend("S"+str(inputType.get())+input_flowrate.get())
    else:
        # Selected Pressure
        serialSend("S"+str(inputType.get())+input_pressure.get())

def p_load_cells(event):
    if GUI_mode.get() == "User Mode":
        serialSend("C")

def r_load_cells(event):
    if GUI_mode.get() == "User Mode":
        serialSend("CR")

def p_run_pump(event):
    if GUI_mode.get() == "User Mode":
        serialSend("P")

def r_run_pump(event):
    if GUI_mode.get() == "User Mode":
        serialSend("PR")

def p_vent_p_res(event):
    if GUI_mode.get() == "User Mode":
        serialSend("A")

def r_vent_p_res(event):
    if GUI_mode.get() == "User Mode":
        serialSend("AR")

def p_vent_c_res(event):
    if GUI_mode.get() == "User Mode":
        serialSend("B")

def r_vent_c_res(event):
    if GUI_mode.get() == "User Mode":
        serialSend("BR")

def set_prop_valve():
    serialSend("V"+str(input_prop.get()))

def admin_mode():
    global GUI_mode
    if (GUI_mode.get() == "Administrator Mode"):
        b_load_cells["state"] = "normal"
        b_run_pump["state"] = "normal"
        b_vent_p_res["state"] = "normal"
        b_vent_c_res["state"] = "normal"
        prop_label.config(fg="black")
        prop_value_label.config(fg="black")
        input_prop["state"] = "normal"
        prop_set["state"] = "normal"
        GUI_mode.set("User Mode")
    else:
        b_load_cells["state"] = "disabled"
        b_run_pump["state"] = "disabled"
        b_vent_p_res["state"] = "disabled"
        b_vent_c_res["state"] = "disabled"
        prop_label.config(fg="gray")
        prop_value_label.config(fg="gray")
        input_prop["state"] = "disabled"
        prop_set["state"] = "disabled"
        GUI_mode.set("Administrator Mode")
        

# # Connect serial with Arduino
# ser = serial.Serial('com7', 9600)
# print("Reset Arduino")
# time.sleep(3)

# Make GUI
tkTop = tk.Tk()
tkTop.title("Pump-on-a-Chip   v.1.1")

# Global variables
inputType = tk.IntVar()
status = tk.StringVar()             # To-do
status.set("Ready")
resP = tk.DoubleVar()               # To-do
cellP = tk.DoubleVar()              # To-do
flowrate = tk.DoubleVar()           # To-do
prop_valve = tk.IntVar()            # To-do
print_prop_valve = tk.StringVar()
input_prop = tk.IntVar()
GUI_mode = tk.StringVar()
GUI_mode.set("Administrator Mode")

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
).grid(row=1,column=1)
input_pressure = tk.Entry(tkTop,
    width=13,
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
).grid(row=2,column=1)
input_flowrate = tk.Entry(tkTop,
    width=13,
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
    padx=20,
    pady=20
).grid(row=1,column=4,rowspan=2)

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
ad_buttons_width = 12
ad_buttons_height = 4
b_load_cells = tk.Button(tkTop,
    text="Load\nCells",
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_load_cells.grid(row=5,column=0,rowspan=2)
b_load_cells.bind("<Button-1>", p_load_cells)
b_load_cells.bind("<ButtonRelease-1>", r_load_cells)

b_run_pump = tk.Button(tkTop,
    text="Run\nPump",
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_run_pump.grid(row=5,column=1,rowspan=2)
b_run_pump.bind("<Button-1>", p_run_pump)
b_run_pump.bind("<ButtonRelease-1>", r_run_pump)

b_vent_p_res = tk.Button(tkTop,
    text="Vent\nPressure\nReservoir",
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_vent_p_res.grid(row=5,column=2,rowspan=2)
b_vent_p_res.bind("<Button-1>", p_vent_p_res)
b_vent_p_res.bind("<ButtonRelease-1>", r_vent_p_res)

b_vent_c_res = tk.Button(tkTop,
    text="Vent\nCell\nReservoir",
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_vent_c_res.grid(row=5,column=3,rowspan=2)
b_vent_c_res.bind("<Button-1>", p_vent_c_res)
b_vent_c_res.bind("<ButtonRelease-1>", r_vent_c_res)

prop_label = tk.Label(tkTop,
    text="Proportional Valve",
    font=("Arial", 10),
    fg="gray"
)
prop_label.grid(row=5,column=4)
print_prop_valve.set(str(prop_valve.get())+' %')
prop_value_label = tk.Label(tkTop,
    textvariable=print_prop_valve,
    font=("Arial", 10),
    fg="gray"
)
prop_value_label.grid(row=5,column=5)

input_prop = tk.Entry(tkTop,
    width=8,
    font=("Arial", 10),
    borderwidth=5,
    state="disabled"
)
input_prop.grid(row=6,column=4)
prop_set = tk.Button(tkTop,
    text="set",
    command=set_prop_valve,
    padx=10,
    state="disabled"
)
prop_set.grid(row=6,column=5)

# Admin mode deactivation/activation
tk.Button(tkTop,
    textvariable=GUI_mode,
    command=admin_mode,
    width=20
).grid(row=7,column=2,columnspan=2)

tk.mainloop()