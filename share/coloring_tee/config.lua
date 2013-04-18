
-- Posible colors.
black = 'black'
red = 'red'
green = 'green'
yellow = 'yellow'
blue = 'blue'
magenta = 'magenta'
cyan = 'cyan'
white = 'white'


-- Configuration table.
coloring_tee_config = {
	color_schemes = {

		gcc = {
			error = { 
				searchString = 'error:',
				color = red
			},
			warning = {
				searchString = 'warning:',
				color = yellow
			},
			note = {
				searchString = 'note:',
				color = blue
			},
			requiredFrom = {
				searchString = 'required from',
				color = blue
			},
			inInstantiationOf = {
				searchString = 'In instantiation of',
				color = blue
			},
			inMember = {
				searchString = 'In member',
				color = blue
			},
			fatal = {
				searchString = 'fatal:',
				color = red
			},
			undefined_reference = { 
				searchString = ': undefined reference',
				color = red
			},
			no_rule_to_make_target = {
				searchString = '*** No rule to make target',
				color = red
			}
		},
		logcat = {
			verbose = { 
				searchString = 'V/',
				color = white
			},
			debug = { 
				searchString = 'D/',
				color = blue
			},
			info = { 
				searchString = 'I/',
				color = green
			},
			warning = { 
				searchString = 'W/',
				color = yellow
			},
			error = { 
				searchString = 'E/',
				color = red
			},
			fatal = { 
				searchString = 'F/',
				color = red
			}
		}
	}
}