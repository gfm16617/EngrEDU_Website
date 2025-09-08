module main(
	// Board I/Os
	input		MAX10_CLK1_50,
	input 	[9:0]		SW,
	output	[9:0]		LEDR,
	output	[35:0]	GPIO
);

	// Wires to make the connection easier between main module with the counter
	wire Clock;
	wire Resetn;
	
	assign Clock = MAX10_CLK1_50;
	assign Resetn = SW[0];
	

	// Counter Template
	parameter DIVISOR = 25000000;
	reg [24:0] counter;  // 2^25 = 33,554,432 > 25,000,000
	reg led_out;

	always @(posedge Clock, negedge Resetn)
		if(!Resetn)
		begin
			counter <= 0;
			led_out <= 0;
		end else
		begin
			// Comparator
			if(counter == DIVISOR - 1)
			begin
				counter <= 0;
				led_out <= ~led_out;  // toggle LED
			end else
				counter <= counter + 1;
		end
			
	// Connect the counter to the LED
	assign LEDR[0] = led_out;
	assign GPIO[35] = led_out;

endmodule
