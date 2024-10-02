import socket
import os
from _thread import *

ServerSocket = socket.socket()
host = "0.0.0.0"  
port = 8085
distance = -1

try:
    ServerSocket.bind((host, port))
    print(f"Servidor iniciado en {host}:{port}")
except socket.error as e:
    print(f"Error al enlazar el puerto: {str(e)}")
    exit()

print("Esperando conexiones...")
ServerSocket.listen(5)

def threaded_client(connection):
    global distance
    while True:
        try:
            data = connection.recv(2048).decode('utf-8')
            if not data:
                print("No se recibieron datos, cerrando conexión.")
                break
            print(f"Datos recibidos: {data}")
            
            if data.startswith('distance='):
                distance = int(data[9:])
                print(f"Distancia recibida: {distance}")
                reply = "ACK\n"
            elif data.startswith('GET'):
                reply = str(distance) + "\n" if distance >= 0 else "0\n"
            else:
                print(f"Datos no reconocidos: {data}")
                reply = "Error: Comando no reconocido\n"
            
            connection.sendall(reply.encode('utf-8'))
        except socket.error as e:
            print(f"Error en la conexión: {str(e)}")
            break
    connection.close()
    print("Conexión cerrada")

try:
    while True:
        Client, address = ServerSocket.accept()
        print(f"Conexión desde: {address[0]}:{str(address[1])}")
        start_new_thread(threaded_client, (Client,))
except KeyboardInterrupt:
    print("\nServidor detenido manualmente")
finally:
    ServerSocket.close()
    print("Socket cerrado, fin de programa")
