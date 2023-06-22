import os
import time
import matplotlib.pyplot as plt

device_path = "/dev/button_device"

def write_character_device(data):
    with open(device_path, "w") as device_file:
        device_file.write(data)

write_data = input("Ingresa la entrada a sensar: ")
write_character_device(write_data)
print("Entrada ", write_data, " elegida")

# Configuración inicial del gráfico
plt.ion()  # Modo interactivo
fig, ax = plt.subplots()
x_data = []
y_data = []

# Configuración de estilo de la gráfica
ax.set_title("Señal en tiempo real")
ax.set_xlabel("Tiempo [s]")
ax.set_ylabel("Valor")
line, = ax.plot(x_data, y_data, color="red", linewidth=2, linestyle="-")

while True:
    with open(device_path, "r") as device_file:
        read_data = device_file.read()
        print("Valor de señal:", read_data)
        
        # Actualizar datos de la gráfica
        x_data.append(time.time())  # Usar el tiempo actual como eje x
        y_data.append(int(read_data))  # Convertir el dato leído a entero y usarlo como eje y
        
        # Actualizar gráfico en tiempo real
        line.set_data(x_data, y_data)  # Actualizar datos de la línea
        ax.relim()  # Actualizar límites de los ejes
        ax.autoscale_view()  # Ajustar escala de los ejes automáticamente
        plt.draw()  # Redibujar la gráfica
        plt.pause(0.1)  # Pausar para permitir la actualización en tiempo real
    
    time.sleep(1)
