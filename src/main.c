import time
try:
    import RPi.GPIO as GPIO
except ImportError:
    # Fallback simulation if running off-device for testing
    import mock_gpio as GPIO

# Setup GPIO configuration
LED_PIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.output(LED_PIN, GPIO.LOW)

def run_monitor():
    # Default acceptable range requirements
    min_temp = 68.0
    max_temp = 76.0
    
    temperatures = []
    log_filename = "temperature_log.txt"
    
    print("=== Smart Classroom Comfort Monitor ===")
    print(f"Default Acceptable Range: {min_temp}°F – {max_temp}°F\n")
    
    # Optional requirement: Allow changing the range per new client requirements
    change_range = input("Would you like to change the acceptable temperature range? (y/n): ").strip().lower()
    if change_range == 'y':
        try:
            min_temp = float(input("Enter new minimum temperature (°F): "))
            max_temp = float(input("Enter new maximum temperature (°F): "))
            print(f"Updated Range: {min_temp}°F – {max_temp}°F\n")
        except ValueError:
            print("Invalid input. Using default range.")

    with open(log_filename, "w") as file:
        file.write("Temperature (°F), Status\n")
        
        while True:
            user_input = input("Enter classroom temperature in °F (or type 'done' to finish): ").strip()
            if user_input.lower() == 'done':
                break
                
            try:
                temp = float(user_input)
            except ValueError:
                print("Please enter a valid numeric temperature.")
                continue
                
            temperatures.append(temp)
            
            # Determine status and control LED
            if min_temp <= temp <= max_temp:
                status = "NORMAL"
                GPIO.output(LED_PIN, GPIO.LOW)  # LED OFF
            else:
                status = "WARNING"
                GPIO.output(LED_PIN, GPIO.HIGH) # LED ON
                
            print(f"Recorded: {temp}°F ---> Status: {status} (LED {'ON' if status == 'WARNING' else 'OFF'})")
            
            # Save reading to file
            file.write(f"{temp}°F, {status}\n")

    # Calculate average temperature
    if temperatures:
        avg_temp = sum(temperatures) / len(temperatures)
        print("\n--- Summary ---")
        print(f"Total readings recorded: {len(temperatures)}")
        print(f"Average temperature: {avg_temp:.2f}°F")
        print(f"Data successfully saved to {log_filename}")
    else:
        print("\nNo temperature readings were recorded.")

try:
    run_monitor()
finally:
    GPIO.cleanup()
    print("GPIO cleaned up. Program exited safely.")
