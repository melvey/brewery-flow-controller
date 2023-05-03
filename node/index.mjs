import SerialPort from 'serialport';
import SerialPortStream from '@serialport/stream';

const SERIAL_START = 0x02;
const SERIAL_END = 0x03;
const SERIAL_MESSAGE_LENGTH = 25;

const SERIAL_MSG_CMD = 'M';
const port = '/dev/ttyUSB0';
const baudRate = 9600;
//const baudRate = 1000000;
const deviceSerial = 'A956RV8A';

const deviceId = 'F1';
const setTargetCmd = 'T';
const startChar = 0x02;
const endChar = 0x03;
const serialStart = '02';
const serialEnd = '03';

function setTotal(serialPort, ml) {
	console.log(`Set total to ${ml}ml`);
	const buff = Buffer.alloc(9);
	buff.write(serialStart, 0 , 1, 'hex');
	buff.write(deviceId, 1, 2);
	buff.write(setTargetCmd, 3, 1);
	buff.writeUInt32LE(ml, 4);
	buff.write(serialEnd, 8, 1, 'hex');
	console.log('Sending', buff);
	serialPort.write(buff);
}

// Send buffer one character at a time
function sendBufferWithDelay(serialPort, buffer, delay) {
    console.log('Sending buffer with delay', buffer);
    for (let i = 0; i < buffer.length; i++) {
        setTimeout(() => serialPort.write(buffer.slice(i, i + 1)), i * delay);
    }
}


function processData(data) {
    if (data.length > 24) {
        const device = data.toString('utf8', 1, 3);
        const cmd = data.toString('utf8', 3, 4);

        if (cmd === SERIAL_MSG_CMD) {
            // Print the received message
            const message = data.toString('utf8', 4, 24);
            console.log(`Message from MCU: ${message}`);
        } else {
            const flow = data.readUInt32LE(4);
            const volume = data.readUInt32LE(8);
            const ticks = data.readUInt32LE(12);
            const target = data.readUInt32LE(16);
            const open = data.readUInt32LE(20);
            console.log({ device, cmd, flow, volume, ticks, target, open });
            if (target <= volume) {
                console.log('Get another litre');
                setTimeout(() => setTotal(serialPort, target + 1000), 5000);
            }
        }
    }
}

SerialPort.list().then((devices) => {
	const active = devices.filter((d) => d.serialNumber);
	console.log({active});
	const path = active.find((d) => d.serialNumber === deviceSerial)?.path || port;
	console.log(`Connecting to ${path}`);


	const serialPort = new SerialPort(
		path, 
		{
			baudRate
		}
	)

	serialPort.on('open', (evt) => {
		console.log('open', evt);
		setTotal(serialPort, 1000);
	});

	serialPort.on('error', (err) => {
		console.error('error', err);
	});

    const bufferToSend = Buffer.from([0x02, 0x46, 0x31, 0x54, 0x88, 0x13, 0x00, 0x00, 0x03]);
    //sendBufferWithDelay(serialPort, bufferToSend, 10); // You can adjust the delay as needed
    setTimeout(() => sendBufferWithDelay(serialPort, bufferToSend, 30), 5000);
	//setTimeout(() => setTotal(serialPort, 5000), 5000);

    let receivedData = Buffer.alloc(0);

    serialPort.on('data', (data) => {
        console.log('data', data);
        receivedData = Buffer.concat([receivedData, data]);

        let startCharIndex = receivedData.indexOf(SERIAL_START);
        let endCharIndex = startCharIndex + SERIAL_MESSAGE_LENGTH - 1;

        while(startCharIndex !== -1 && endCharIndex <= receivedData.length) {
            if(receivedData[endCharIndex] === SERIAL_END) {
                const commandData = receivedData.slice(startCharIndex, endCharIndex + 1);
                processData(commandData);
                receivedData = receivedData.slice(endCharIndex + 1);
                startCharIndex = receivedData.indexOf(SERIAL_START);
                endCharIndex = startCharIndex + SERIAL_MESSAGE_LENGTH - 1;
            } else {
                // chop off the start character
                receivedData = receivedData.slice(startCharIndex);
            }
        }
    });
});
