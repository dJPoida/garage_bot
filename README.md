# Garage Bot v1.0
This project adds some form of IoT capability to my existing garage door.

## Back Story
At the time of creating this project, we're renting our house. This limits my ability to implement smart technology or change out hardware. For over two years our landlord has not supplied us with a second remote control for the garage door, meaning my wife and I have to plan who is going to take the remote with us based on who is likely to be home first. Couple that with frequent incidents of accidentally leaving the garage door open at night, and voila, motivation to improve our garage door situation.

## Goals
- Provide 2x remote controls for each car
- Provide a mechanism for opening / closing the garage door from within the house
- Connect the controller to Home Assistant via MQTT
- Enable Google Home integration
    - Open / Close commands
    - Open / Closed query
- Avoid drastic modification of the garage door to ensure we don't break our lease agreement when it's time to turn over the remote controls

## System Design
![System Design](designs/system_overview.png)

## TODO
- User Interface
    - Provide a form to configure the MQTT settings
    - Provide a form to configure the sensitivity of the sensors
- Improve reliability
    - When the client socket disconnects, attempt to re-connect
    - When the WiFi on the device is disconnected, attempt to re-connect