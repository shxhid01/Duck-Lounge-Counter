# Motion Counter System

A dual PIR sensor system built with ESP8266 that tracks directional movement and reports to Discord. Perfect for monitoring foot traffic, room occupancy, or event attendance.

## Features

- Bidirectional motion tracking using two PIR sensors
- Real-time Discord webhook notifications
- Counter system that tracks net movement
- Start/Stop functionality with state preservation
- Serial command interface
- Debounce protection to prevent false triggers
- Automatic sensor disabling to prevent double-counting

## Hardware Requirements

- ESP8266 board (NodeMCU or similar)
- 2x PIR motion sensors
- Power supply
- Jumper wires

## Pin Configuration

- PIR Sensor 1: D0 (GPIO16)
- PIR Sensor 2: D1 (GPIO5)

## Software Dependencies

Install the following libraries through Arduino IDE Library Manager:
- ESP8266WiFi
- ESP8266HTTPClient
- WiFiClientSecure
- ArduinoJson

## Setup Instructions

1. Clone this repository or copy the code files
2. Open the `.ino` file in Arduino IDE
3. Install required libraries
4. Configure your settings:
   ```cpp
   const char* ssid = "your-wifi-ssid";
   const char* password = "your-wifi-password";
   const char* discordWebhookURL = "your-discord-webhook-url";
   ```
5. Upload to your ESP8266 board

## Discord Setup

1. Create a Discord server (or use existing)
2. Create a webhook:
   - Server Settings → Integrations → Create Webhook
   - Copy the webhook URL
   - Paste into the code's `discordWebhookURL` variable

## Usage

### Serial Commands

- `start` - Start the sensor system
- `stop` - Stop the sensor system (preserves counter)
- `reset` - Reset counter to zero
- `status` - Display system status (coming soon)

### Discord Notifications

The system sends notifications for:
- Motion detection events
- System start/stop
- Counter resets
- Error states

### LED Indicators

- Built-in LED flashes on motion detection
- Solid LED indicates system stopped

## Counter Logic

- Sensor 1 trigger: +1 to counter
- Sensor 2 trigger: -1 from counter
- 2-second debounce delay between triggers
- Automatic opposite sensor disable to prevent double counting

## Troubleshooting

Common issues and solutions:

1. No Discord messages:
   - Verify webhook URL
   - Check WiFi connection
   - Confirm Discord server permissions

2. False triggers:
   - Adjust sensor positioning
   - Increase debounce delay
   - Check for heat/AC interference

3. Missing counts:
   - Verify sensor placement
   - Check power supply stability
   - Reduce sensor spacing

## Future Enhancements

Planned features:
- Data logging capabilities
- Web interface for configuration
- Time-based auto reset
- Advanced statistics reporting
- Remote control via Discord
- Customizable notification settings

## Contributing

Feel free to fork, submit PRs, or suggest features through issues.

## License

MIT License - Feel free to use and modify for your needs.

## Safety Notes

- Mount sensors securely
- Use appropriate power supply
- Keep electronics away from moisture
- Follow local privacy regulations regarding motion detection

## Support

For issues:
1. Check troubleshooting section
2. Review serial monitor output
3. Create GitHub issue with:
   - Full error message
   - Hardware configuration
   - Steps to reproduce