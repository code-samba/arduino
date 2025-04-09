import serial
import time

serialPort = '/dev/rfcomm0'
baudrate = 9600

try:
    arduino = serial.Serial(serialPort, baudrate, timeout=1)
    time.sleep(2)

    print("Lendo dados do Arduino...")
    while True:
        if arduino.in_waiting > 0:
            line = arduino.readline().decode('utf-8').strip()
            print("Recebido bruto: ", line)

            data = line.split(',')
            arduinoData = {}
            for item in data:
                if ':' in item:
                    key, value = item.split(':')
                    arduinoData[key] = float(value) if '.' in value or value.isdigit() else value

            print("Temperatura:", arduinoData.get("TEMP"), "°C")
            print("Luminosidade:", arduinoData.get("LUX"), "lux")
            print("Umidade:", arduinoData.get("UMIDADE"))
            print("Lâmpada ligada?", "Sim" if arduinoData.get("LAMP") == 1 else "Não")
            print("Bomba ligada?", "Sim" if arduinoData.get("BOMB") == 1 else "Não")
            print("-" * 40)

except serial.SerialException as e:
    print("Erro ao acessar a porta serial:", e)

except KeyboardInterrupt:
    print("\nEncerrando a leitura.")
