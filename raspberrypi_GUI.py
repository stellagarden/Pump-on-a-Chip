#  -----------------------------------------------------------
#  FileName     : raspberrypi_GUI.ino
#  Author       : Hyemin Stella Lee
#  Created      : 06/15/2023
#  Description  : GUI for controlling Pump-on-a-Chip
# -----------------------------------------------------------

import serial
import time
import tkinter as tk
import threading
import datetime

# Define buttons
def serialSend(a):
    ser.write(bytes(a,'UTF-8'))
    print("[", datetime.datetime.now(), "] Send: "+str(a))

def start():
    global status
    # Change status
    status.set("Pressurizing")
    display_status.config(fg="black",bg="white")

    # Send starting sign, inputType, and input to Arduino
    if (inputType.get()):
        # Selected Flowrate
        serialSend("S"+str(inputType.get())+input_flowrate.get())
    else:
        # Selected Pressure
        serialSend("S"+str(inputType.get())+input_pressure.get())

    # Deactivate inputs
    rb_pressure["state"] = "disabled"
    input_pressure["state"] = "disabled"
    unit_pressure.config(fg="gray")
    rb_flowrate["state"] = "disabled"
    input_flowrate["state"] = "disabled"
    unit_flowrate.config(fg="gray")
    b_start["state"] = "disabled"

def load_cells():
    if GUI_mode.get() == "User Mode":
        serialSend("C")

def run_pump():
    if GUI_mode.get() == "User Mode":
        serialSend("P")

def vent_p_res():
    if GUI_mode.get() == "User Mode":
        serialSend("A")

def vent_c_res():
    if GUI_mode.get() == "User Mode":
        serialSend("B")

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
        
def arduino_handler():
    global status
    while True:
        data = ser.readline().decode().strip()
        # print("Received: "+data)
        match data[0]:
            # Sensor values update
            case "P":
                resP.set(float(data[1:]))
            case "C":
                cellP.set(float(data[1:]))
            case "F":
                flowrate.set(float(data[1:]))
            case "V":
                if GUI_mode.get() == "User Mode":
                    prop_valve.set(float(data[1:]))
                    print_prop_valve.set(str(prop_valve.get())+' %')
            # State change
            case "S":
                match data[1]:
                    case "2":
                        status.set("Cell Loading")
                        display_status.config(fg="black",bg="white")
                    case "3":
                        status.set("Running")
                        display_status.config(fg="red",bg="white")
                    case "4":
                        status.set("Finish")
                        display_status.config(fg="blue",bg="white")
                    case "0":
                        status.set("Ready")
                        display_status.config(fg="white",bg="green")
                        # Activate inputs
                        rb_pressure["state"] = "normal"
                        input_pressure["state"] = "normal"
                        unit_pressure.config(fg="black")
                        rb_flowrate["state"] = "normal"
                        input_flowrate["state"] = "normal"
                        unit_flowrate.config(fg="black")
                        b_start["state"] = "normal"
            case _:
                print("[", datetime.datetime.now(), "] Receive: "+data)

# Connect serial with Arduino
ser = serial.Serial('COM6', 9600)
print("Reset Arduino")
time.sleep(3)

# Make GUI
tkTop = tk.Tk()
tkTop.title("Pump-on-a-Chip   v.1.1")

# Global variables
inputType = tk.IntVar()
status = tk.StringVar()
status.set("Ready")
resP = tk.DoubleVar()
resP.set(0.0)
cellP = tk.DoubleVar()
cellP.set(0.0)
flowrate = tk.DoubleVar()
flowrate.set(0.0)
prop_valve = tk.DoubleVar()
print_prop_valve = tk.StringVar()
input_prop = tk.IntVar()
GUI_mode = tk.StringVar()
GUI_mode.set("Administrator Mode")

# Status
display_status = tk.Label(tkTop,
    textvariable=status,
    font=("Arial", 20),
    padx=40,
    pady=5,
    relief="solid",
    fg="white",
    bg="green",
    width=20,
    height=1
)
display_status.grid(row=0,column=0,columnspan=6)

# Initial input - Pressure
rb_pressure = tk.Radiobutton(tkTop,
    text="Pressure: ",
    font=("Arial", 10),
    variable=inputType,
    value=0
)
rb_pressure.grid(row=1,column=1)
input_pressure = tk.Entry(tkTop,
    width=13,
    font=("Arial", 10),
    borderwidth=5
)
input_pressure.grid(row=1,column=2)
unit_pressure = tk.Label(tkTop,
    text="mbar",
    font=("Arial", 10)
)
unit_pressure.grid(row=1,column=3)

# Initial input - flowrate
rb_flowrate = tk.Radiobutton(tkTop,
    text="Flowrate: ",
    font=("Arial", 10),
    variable=inputType,
    value=1
)
rb_flowrate.grid(row=2,column=1)
input_flowrate = tk.Entry(tkTop,
    width=13,
    font=("Arial", 10),
    borderwidth=5
)
input_flowrate.grid(row=2,column=2)
unit_flowrate = tk.Label(tkTop,
    text="ml/min",
    font=("Arial", 10)
)
unit_flowrate.grid(row=2,column=3)
# flow rate input is deactivated for this point
rb_flowrate["state"] = "disabled"
input_flowrate["state"] = "disabled"
unit_flowrate.config(fg="gray")

# Start button
b_start = tk.Button(tkTop,
    text="START",
    command=start,
    padx=20,
    pady=20
)
b_start.grid(row=1,column=4,rowspan=2)

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
    command=load_cells,
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_load_cells.grid(row=5,column=0,rowspan=2)

b_run_pump = tk.Button(tkTop,
    text="Run\nPump",
    command=run_pump,
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_run_pump.grid(row=5,column=1,rowspan=2)

b_vent_p_res = tk.Button(tkTop,
    text="Vent\nPressure\nReservoir",
    width=ad_buttons_width,
    command=vent_p_res,
    height=ad_buttons_height,
    state="disabled"
)
b_vent_p_res.grid(row=5,column=2,rowspan=2)

b_vent_c_res = tk.Button(tkTop,
    text="Vent\nCell\nReservoir",
    command=vent_c_res,
    width=ad_buttons_width,
    height=ad_buttons_height,
    state="disabled"
)
b_vent_c_res.grid(row=5,column=3,rowspan=2)

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
    width=7,
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

threading.Thread(target=arduino_handler, daemon=True).start()
tk.mainloop()