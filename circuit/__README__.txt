
tools: 
  electronic_drawing_simulator.txt  - overview of drawing and simulator tools used to create the models in this folder

documentation: 
    high_low_voltages.txt
    pullup_pulldown_resister_on_input_gpio.txt
    safety_when_connecting_electronic_circuits_to_the_raspberrypi.txt


models:

 - basic 
    led.icircuit            - iCircuit model of LED circuit
    led.png                 - png of iCircuit model
    pullup.icircuit         - iCircuit model of pull up circuit
    pullup.png              - png of iCircuit model
    pulldown.icircuit       - iCircuit model of pull down circuit
    pulldown.png            - png of iCircuit model

 - advanced  
    button_toggles_led.fzz  - Fritzing model of circuit in which button press toggles the led (detects button on gpio 23 input, drives led on output gpio 22)
    button_toggles_led.png  - png of Fritzing model
    
    output_driven_irq.fzz   - Fritzing model of circuit in which an data output line  gpio 22 causes toggling of a led and an interupt on gpio 24 input line
    output_driven_irq.png   - png of Fritzing model

 - combined 
    combined__button_toggles_led__output_driven_irq.fzz  - combined Fritzing model of above 2 circuits  
    combined__button_toggles_led__output_driven_irq.png  - png of Fritzing model

 
 IMPORTANT: 
   using the combined model one can test all examples in this repository !

