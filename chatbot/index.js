const dotenv = require('dotenv').config()
const tmi= require('tmi.js')
const color = require('color')
const mqtt = require('mqtt')

// Valid commands start with:
let commandPrefix = '!'
// Define configuration options:
let opts = {
  identity: {
    username: 'nodebotanist-lights',
    password: 'oauth:' + process.env.TWITCH_IRC_PASSWORD
  },
  channels: [
    '#nodebotanist'
  ]
}

// Create a client with our options:
let client = new tmi.client(opts)

let mqttClient = mqtt.connect('https://io.adafruit.com/nodebotanist/feeds/colorbot', {
  username: 'nodebotanist',
  password: process.env.AIO_KEY,
  port: 1883
})

mqttClient.on('connect', () => {
  console.log('connected!')
})

mqttClient.on('error', (error) => {
  console.log(error)
})

// Register our event handlers (defined below):
client.on('message', onMessageHandler)
client.on('connected', onConnectedHandler)
client.on('disconnected', onDisconnectedHandler)

// Connect to Twitch:
client.connect()

let knownCommands = {
  'hello': (target, context, params) => {
    client.action("#nodebotanist", "Hi!");
  },
  'color': (target, context, params) => {
    let lightColor = null
    try {
      lightColor = color(params[0])      
    } catch (error) {}
    console.log(lightColor)
    if(lightColor){
      client.action('#nodebotanist', `Color R:${lightColor.color[0]} G:${lightColor.color[1]} B:${lightColor.color[2]}!`)
    } else {
      client.action('#nodebotanist', 'Invalid color!')
    }
  }
}

// Called every time a message comes in:
function onMessageHandler (target, context, msg, self) {
  if (self) { return } // Ignore messages from the bot

  // This isn't a command since it has no prefix:
  if (msg.substr(0, 1) !== commandPrefix) {
    console.log(`[${target} (${context['message-type']})] ${context.username}: ${msg}`)
    return
  }

  // Split the message into individual words:
  const parse = msg.slice(1).split(' ')
  // The command name is the first (0th) one:
  const commandName = parse[0]
  // The rest (if any) are the parameters:
  const params = parse.splice(1)

  // If the command is known, let's execute it:
  if (commandName in knownCommands) {
    // Retrieve the function by its name:
    const command = knownCommands[commandName]
    // Then call the command with parameters:
    command(target, context, params)
    console.log(`* Executed ${commandName} command for ${context.username}`)
  } else {
    console.log(`* Unknown command ${commandName} from ${context.username}`)
  }
}

// Called every time the bot connects to Twitch chat:
function onConnectedHandler (addr, port) {
  console.log(`* Connected to ${addr}:${port}`)
}

// Called every time the bot disconnects from Twitch:
function onDisconnectedHandler (reason) {
  console.log(`Womp womp, disconnected: ${reason}`)
  process.exit(1)
}