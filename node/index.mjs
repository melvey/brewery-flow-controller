import SerialPort from 'serialport';
import SerialPortStream from '@serialport/stream';

const port = '/dev/ttyUSB0';
const baudRate = 1000000;

SerialPort.list().then(console.log);

const serialPort = new SerialPort(
	port, 
	{
		baudRate
	}
)

serialPort.on('open', () => console.log('open'));

serialPort.on('data', (data) => {
	console.log({data})
	console.log(data.length);
	if(data.length > 24) {
		const device = data.toString('utf8', 1, 3);
		const cmd = data.toString('utf8', 3, 4);
		const flow = data.readUInt32LE(4);
		const volume = data.readUInt32LE(8);
		const ticks = data.readUInt32LE(12);
		const target = data.readUInt32LE(16);
		const open = data.readUInt32LE(20);
		console.log({device, cmd, flow, volume, ticks, target, open});
	}
});
