const http = require('http');

const PORT = 8001;


const server = http.createServer((req, res) => {

	const rawHeader = req.rawHeaders;

	//print body
	let body = '';
	req.on('data', (chunk) => {
		body += chunk;
	});


	console.log(rawHeader);
	console.log(body);

	res.writeHead(200, { 'Content-Type': 'text/plain' });
	res.end('Hello World\n');
})


server.listen(PORT, () => {
	console.log(`Server running at http://localhost:${PORT}/`);
});

