var rocky = require('rocky');

var DAY_NAMES = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

// An object to cache our date & time values,
// to minimize computations in the draw handler.
var clockData = {
  time: '',
  date: '',
  timestamp: '',
  weekday: '',
};

// https://developer.pebble.com/docs/rockyjs/rocky/#on
rocky.on('minutechange', function(event) {
  // Current date/time
  // https://developer.pebble.com/docs/rockyjs/Date/
  var d = event.date;

  // Get current time, based on 12h or 24h format (01:00 or 1:00 AM)
  clockData.time = d.toLocaleTimeString().replace(/:\d+($| )/, '$1');

  // Day of month
  var day = d.toLocaleDateString(undefined, {day: 'numeric'});

  // Month name
  var month = d.toLocaleDateString(undefined, {month: 'long'});

  // Date
  clockData.date = (day + ' ' + month);

  // weekday
  clockData.weekday = DAY_NAMES[d.getDay()];

  // Timestamp
  clockData.timestamp = ~~(d.getTime() / 1000);

  // Force screen redraw
  rocky.requestDraw();
});

// Redraw the screen
rocky.on('draw', function(event) {
  var ctx = event.context;

  var cw = ctx.canvas.clientWidth;
  var ch = ctx.canvas.clientHeight;
  var vw = ctx.canvas.unobstructedWidth;
  var vh = ctx.canvas.unobstructedHeight;

  ctx.clearRect(0, 0, cw, ch);

  var offsetY = ch - vh;
  var centerX = vw / 2;

  ctx.fillStyle = 'white';

  // Timestamp
  ctx.textAlign = 'left';
  ctx.font = '14px Gothic';
  ctx.fillText(clockData.timestamp, 6, 6 - offsetY);

  // Date
  ctx.textAlign = 'left';
  ctx.font = '18px bold Gothic';
  ctx.fillText(clockData.date, 6, vh - 62);

  // Weekday
  ctx.textAlign = 'right';
  ctx.font = '14px bold Gothic';
  ctx.fillText(clockData.weekday, vw - 6, vh - 58);

  // Line
  ctx.rect(6, vh - 40, vw - 12, 2);
  ctx.fill();

  // Time
  ctx.textAlign = 'center';
  ctx.font = '49px Roboto-subset';
  ctx.fillText(clockData.time, centerX, vh - 50);
});


// Send a single message to the Phone
// https://developer.pebble.com/docs/rockyjs/rocky/#postMessage
rocky.postMessage("This arrives on the phone via bluetooth!");
