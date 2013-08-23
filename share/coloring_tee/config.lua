
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
			-- For all.
			noSuchFileOrDirectory = {
				searchString = ': No such file or directory',
				color = red
			},
			-- gcc.
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
			unimplemented = {
				searchString = 'sorry, unimplemented:',
				color = red
			},
			-- ld.
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
			inFunction = {
				searchString = ': In function',
				color = blue
			},
			multipleDefinitions = {
				searchString = ': multiple definition',
				color = red
			},
			firstDefinedHere = {
				searchString = ': first defined here',
				color = blue
			},
			cannotFind = {
				searchString = ': cannot find',
				color = red
			},
			-- Makefile.
			no_rule_to_make_target = {
				searchString = '*** No rule to make target',
				color = red
			},
			-- ndk-build.
			WARNING = {
				searchString = 'WARNING:',
				color = yellow
			},
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
			},
		},
		bracket = {
			verbose = { 
				searchString = '[verbose]',
				color = white
			},
			debug = { 
				searchString = '[debug]',
				color = blue
			},
			info = { 
				searchString = '[info]',
				color = green
			},
			warning = { 
				searchString = '[warning]',
				color = yellow
			},
			error = { 
				searchString = '[error]',
				color = red
			},
			critical = { 
				searchString = '[critical]',
				color = red
			},
			fatal = { 
				searchString = '[fatal]',
				color = red
			},
		}		
	}
}
