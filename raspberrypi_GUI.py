import serial
import time
import tkinter as tk

# Define buttons
def quit():
    global tkTop
    # ser.write(bytes('L', 'UTF-8'))
    tkTop.destroy()

def set_button1_state(): 
        varLabel.set("LED ON ")
        # ser.write(bytes('H', 'UTF-8'))
        varLabel2.set(b)
        print(b)

def set_button2_state():
        varLabel.set("LED OFF")
        # ser.write(bytes('L', 'UTF-8'))

# Connect serial with Arduino
# ser = serial.Serial('com7', 9600)
# print("Reset Arduino")
# time.sleep(3)
# ser.write(bytes('L', 'UTF-8'))

# Make GUI
tkTop = tk.Tk()
tkTop.geometry('400x400')
tkTop.title("Pump-on-a-Chip v.1.1")

status = tk.StringVar()
display_status = tk.Label(tkTop,
    textvariable=status,
    font=("Arial", 20),
    borderwidth=1,
    relief="solid",
    fg="black",
    bg="green",
    width=20,
    height=1
)
status.set("Ready")
display_status.pack(pady=10)

varLabel = tk.StringVar()
tkLabel = tk.Label(textvariable=varLabel, )
tkLabel.pack()

varLabel2 = tk.IntVar()
tkLabel2 = tk.Label(textvariable=varLabel2, )
tkLabel2.pack()

button1 = tk.IntVar()
button1state = tk.Button(tkTop,
    text="ON",
    command=set_button1_state,
    height = 4,
    fg = "black",
    width = 8,
    bd = 5,
    activebackground='green'
)
button1state.pack(side='top', ipadx=10, padx=10, pady=15)

button2 = tk.IntVar()
button2state = tk.Button(tkTop,
    text="OFF",
    command=set_button2_state,
    height = 4,
    fg = "black",
    width = 8,
    bd = 5
)
button2state.pack(side='top', ipadx=10, padx=10, pady=15)

tkButtonQuit = tk.Button(
    tkTop,
    text="Quit",
    command=quit,
    height = 4,
    fg = "black",
    width = 8,
    bg = 'yellow',
    bd = 5
)
tkButtonQuit.pack(side='top', ipadx=10, padx=10, pady=15)

tk.mainloop()