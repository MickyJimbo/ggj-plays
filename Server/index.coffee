express = require('express')
app = express()
http = require("http").createServer(app);
io = require( "socket.io" )( http );

http.listen 80

app.get '/', (req, res) ->
  res.sendFile __dirname + '/index.html'

net = require "net"

port = 6767

gameConnected = false

userCount = 0

send = () ->

TCPServer = net.createServer()

TCPServer.on 'connection', (socket) ->
  console.log "Connected!"
  gameConnected = true

  send = (obj) ->
    setImmediate () ->
      # creates a string buffer
      stringBuffer = Buffer.from JSON.stringify obj
      # creates a buffer containing 4 bytes
      byteSizeBuffer = Buffer.alloc(4, 0)
      # writes a Little Endian Int32 to the buffer describing the number of bytes in the string buffer
      byteSizeBuffer.writeInt32LE stringBuffer.length
      # concatinates the two buffers into a new buffer
      sendBuffer = Buffer.concat [ byteSizeBuffer, stringBuffer ]
      # send the buffer to the client
      socket.write sendBuffer
    return

  # event for when data is received.
  socket.on 'data', (data) ->

  # simple socket error management (prevents crashes on uncaught exceptions)
  socket.on 'error', (error) ->
    console.error error
    return

  # socket close clean-up
  socket.on 'close', (error) ->
    return

  return

TCPServer.listen port, "127.0.0.1"

io.on 'connection', (socket) ->

  userCount++

  setInterval () ->
    if gameConnected
      send userCount
  , 1000

  socket.on 'direction', (data) ->
    if gameConnected
      if data.direction is "left"
        send "left"
      if data.direction is "right"
        send "right"

  socket.on 'disconnect', () ->
    userCount--
