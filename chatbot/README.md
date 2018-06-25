# Installation of the Chat Bot

1. If you don't have it already, install [Node.JS and npm](nodejs.org), this bot requires at least Node.JS 8.x.x
1. run `npm i` to install all dependencies
1. You'll need a Twitch IRC password, you can learn how to get one [from the docs](https://dev.twitch.tv/docs/authentication/) or they have a quick-use [password generator](https://twitchapps.com/tmi/)
1. You will also need an [Adafruit.IO](https://adafruit.io) account. They have a free tier. You will need to copy your adafruit.io key (click the 'View AIO Key' link in the left menu), and the feed you will use to communicate with your bot. On the feed page, click 'Feed Information' on the right and copy the value for 'MQTT by Key'.
1. Create a .env file in the chatbot directory that contains the following:

```
TWITCH_IRC_PASSWORD=your_twitch_IRC_password 
TWITCH_IRC_ROOM=usually_your_twitch_username
TWITCH_USERNAME=your_twitch_username
AIO_KEY=your_adafruit_IO_key
AIO_FEED=your_adafruit_IO_MQTT_key
```

1. run `node index.js` to run the chat bot. Make sure the [hardware](../botbot/README.md) is running as well!