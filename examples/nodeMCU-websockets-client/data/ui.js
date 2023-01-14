let ui = {
	"title": "Arduino Universal Controler",
	"header": {
		"class": "mdl-layout--fixed-header",
		"drawer": {
			"title": "Arduino Universal Controler",
			"links": [
				{"name": "Digital", "icon": "pin_invoke", "id": "digitalMenu", "class": "", "link": "#digital"},
				{"name": "Analog", "icon": "timeline", "id": "analogMenu", "class": "", "link": "#analog"},
				{"name": "Led", "icon": "wb_incandescent", "id": "ledMenu", "class": "", "link": "#led"},
				{"name": "Audio", "icon": "volume_up", "id": "audioMenu", "class": "", "link": "#audio"},
				{"spacer": true},
				{"name": "t6 IoT App", "icon": "api", "id": "t6iot", "class": "", "link": "https://api.internetcollaboratif.info"},
				{"name": "t6 Api doc", "icon": "integration_instructions", "id": "apidoc", "class": "", "link": "https://doc.internetcollaboratif.info"},
				{"spacer": true},
				{"name": "Settings", "icon": "settings", "id": "settings", "class": "", "link": "#settings"},
				{"name": "Help", "icon": "contact_support", "id": "help", "class": "", "link": "#help"},
				{"spacer": true},
				{"name": "Reboot ESP", "icon": "power", "id": "reboot", "class": "", "link": "/reboot"},
				{"spacer": true}
			],
			"text": {
				"id": "credit",
				"text": "t6 IoT © 2022 - v1.0.0"
			}
		},
		"links": [
			{"name": "t6 IoT App", "id": "t6iot", "class": "", "link": "https://api.internetcollaboratif.info"},
			{"name": "t6 Api doc", "id": "apidoc", "class": "", "link": "https://doc.internetcollaboratif.info"}
		]
	},
	"tab_contents":[
		{
			"name": "Digital",
			"id": "digital",
			"class": "is-active",
			"icon": "pin_invoke",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab1_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab1_row1_col1",
								"cards":[
									{
										"width":6,
										"title":"Digital Write",
										"icon": "pin_invoke",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D0",
													"body":"HIGH during boot and LOW for programming",
													"switches":[
														{
															"id":"pin0",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=0&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin TX0",
													"body":"Tx pin, used for flashing and debugging",
													"switches":[
														{
															"id":"pin1",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=1&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D2",
													"body":"LOW during boot and also connected to the on-board LED",
													"switches":[
														{
															"id":"pin2",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=2&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin RX0",
													"body":"Rx pin, used for flashing and debugging",
													"switches":[
														{
															"id":"pin3",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=3&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D4",
													"body":"",
													"switches":[
														{
															"id":"pin4",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=4&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D5",
													"body":"HIGH during boot",
													"switches":[
														{
															"id":"pin5",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=5&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D6",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin6",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=6&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D7",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin7",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=7&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D8",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin8",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=8&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D9",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin9",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=9&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D10",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin10",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=10&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D11",
													"body":"Connected to Flash memory",
													"switches":[
														{
															"id":"pin11",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=11&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D12",
													"body":"LOW during boot",
													"switches":[
														{
															"id":"pin12",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=12&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D13",
													"body":"",
													"switches":[
														{
															"id":"pin13",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=13&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D14",
													"body":"",
													"switches":[
														{
															"id":"pin14",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=14&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D15",
													"body":"HIGH during boot, prevents startup log if pulled LOW",
													"switches":[
														{
															"id":"pin15",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=15&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin RX2",
													"body":"",
													"switches":[
														{
															"id":"pin16",
															"label":"",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=16&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin TX2",
													"body":"",
													"switches":[
														{
															"id":"pin17",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=17&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D18",
													"body":"",
													"switches":[
														{
															"id":"pin18",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=18&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D19",
													"body":"",
													"switches":[
														{
															"id":"pin19",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=19&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D21",
													"body":"",
													"switches":[
														{
															"id":"pin21",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=21&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D22",
													"body":"",
													"switches":[
														{
															"id":"pin22",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=22&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D23",
													"body":"",
													"switches":[
														{
															"id":"pin23",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=23&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D25",
													"body":"",
													"switches":[
														{
															"id":"pin25",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=25&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D26",
													"body":"",
													"switches":[
														{
															"id":"pin26",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=26&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D27",
													"body":"",
													"switches":[
														{
															"id":"pin27",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=27&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D32",
													"body":"",
													"switches":[
														{
															"id":"pin32",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=32&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D33",
													"body":"",
													"switches":[
														{
															"id":"pin33",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=33&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D34",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin34",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=34&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin D35",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin35",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=35&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin VP",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin36",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=36&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"Digital Pin VN",
													"body":"Input only GPIO, cannot be configured as output",
													"switches":[
														{
															"id":"pin39",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=39&value=%s"
														}
													],
													"class": "mdl-list__item-avatar"
												}
											]
										},
										"actions":{}
									},
									{
										"width":6,
										"title":"digital Read",
										"icon": "pin_invoke",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"pin D0",
													"body":"",
													"body_id":"trigger_pinD0",
													"buttons": [
														{
															"id":"pinD0",
															"label":"Read pin D0",
															"action":"/digitalRead?pin=0",
															"trigger":"trigger_pinD0",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D1",
													"body":"",
													"body_id":"trigger_pinD1",
													"buttons": [
														{
															"id":"pinD1",
															"label":"Read pin D1",
															"action":"/digitalRead?pin=1",
															"trigger":"trigger_pinD1",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D2",
													"body":"",
													"body_id":"trigger_pinD2",
													"buttons": [
														{
															"id":"pinD2",
															"label":"Read pin 2",
															"action":"/digitalRead?pin=2",
															"trigger":"trigger_pinD2",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin RX0",
													"body":"",
													"body_id":"trigger_pinD3",
													"buttons": [
														{
															"id":"pinD3",
															"label":"Read pin 3",
															"action":"/digitalRead?pin=3",
															"trigger":"trigger_pinD3",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D4",
													"body":"",
													"body_id":"trigger_pinD4",
													"buttons": [
														{
															"id":"pinD4",
															"label":"Read pin 4",
															"action":"/digitalRead?pin=4",
															"trigger":"trigger_pinD4",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D5",
													"body":"",
													"body_id":"trigger_pinD5",
													"buttons": [
														{
															"id":"pinD5",
															"label":"Read pin 5",
															"action":"/digitalRead?pin=5",
															"trigger":"trigger_pinD5",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D6",
													"body":"",
													"body_id":"trigger_pinD6",
													"buttons": [
														{
															"id":"pinD6",
															"label":"Read pin 6",
															"action":"/digitalRead?pin=6",
															"trigger":"trigger_pinD6",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D7",
													"body":"",
													"body_id":"trigger_pinD7",
													"buttons": [
														{
															"id":"pinD7",
															"label":"Read pin 7",
															"action":"/digitalRead?pin=7",
															"trigger":"trigger_pinD7",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												},
												{
													"icon": "pin_invoke",
													"label":"pin D8",
													"body":"",
													"body_id":"trigger_pinD8",
													"buttons": [
														{
															"id":"pinD8",
															"label":"Read pin 8",
															"action":"/digitalRead?pin=8",
															"trigger":"trigger_pinD8",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Analog",
			"id": "analog",
			"class": "",
			"icon": "timeline",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab2_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"C",
								"cards":[
									{
										"width":4,
										"title":"Analog",
										"icon": "timeline",
										"body":{
											"slider":
												{
													"label":"A0",
													"id":"sliderA0",
													"type":"range",
													"width":12,
													"min":0,
													"max":100,
													"value":0,
													"step":1,
													"unit":"%s %",
													"action":"/analogWrite?pin=0&value=%s"
												}
										},
										"actions":{}
									},
									{
										"width":8,
										"title":"analogRead",
										"icon": "timeline",
										"body":{
											"lists": [
												{
													"icon": "timeline",
													"label":"n/a",
													"body":"",
													"body_id":"trigger_pinA0",
													"buttons": [
														{
															"id":"pinA0",
															"label":"analogRead 0",
															"action":"/analogRead?pin=0",
															"trigger":"trigger_pinA0",
															"class":"mdl-button--raised"
														}
													],
													"class": "mdl-list__item-avatar"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Led RGB",
			"id": "ledrgb",
			"class": "",
			"icon": "wb_incandescent",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab3_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab3_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"Led color",
										"icon": "wb_incandescent",
										"body":{
											"texts":[
												{
													"text":"Choose Led color from sliders below:",
													"width":12,
													"id":"",
													"class":""
												}
											],
											"sliders":[
												{
													"label":"Red",
													"id":"sliderRed",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=5&value=%s"
												},
												{
													"label":"Green",
													"id":"sliderGreen",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=4&value=%s"
												},
												{
													"label":"Blue",
													"id":"sliderBlue",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":5,
													"unit":"",
													"action":"/analogWrite?pin=16&value=%s"
												}
											]
										},
										"actions":{
											"buttons":[
												{
													"label":"get mode and values",
													"value":"",
													"action":"/getValues?pin=0,1,2,3,4,5,6,7,8,9",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode R",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=4",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode G",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=5",
													"trigger":"sensorValue"
												},
												{
													"label":"pinMode B",
													"value":"OUTPUT",
													"action":"/setPinModeOutput?pin=16",
													"trigger":"sensorValue"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Arduino Audio",
			"id": "audio",
			"class": "",
			"icon": "volume_up",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab4_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab4_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"Arduino Audio",
										"icon": "volume_up",
										"body":{
											"inputs":[
												{
													"label":"Say something:",
													"icon":"record_voice_over",
													"placeholder":"Type your message to say here...",
													"pattern": "",
													"error":"",
													"width":12,
													"id":"audioOutput",
													"class":""
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Say",
													"value":"",
													"icon":"record_voice_over",
													"action":"/audioOutput?value=%s",
													"trigger":"audioOutput",
													"class":"mdl-button--raised"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Settings",
			"id": "settings",
			"class": "",
			"icon": "settings",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab7_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab7_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"Object Wifi settings",
										"icon":"wifi",
										"body":{
											"sections":[
												{
													"title":"Wifi",
													"width":12,
													"id":"wifi_section",
													"content": {
														"inputs":[
															{
																"label":"Wifi SSID",
																"icon":"wifi",
																"placeholder":"ssid",
																"pattern": "([^ ]+$)",
																"error":"Can't have a space'",
																"width":12,
																"expandable":false,
																"id":"config.wifi.ssid",
																"class":""
															},
															{
																"label":"Wifi password",
																"icon":"lock",
																"placeholder":"password",
																"pattern": "([^\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.wifi.password",
																"class":""
															}
														],
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.wifi",
													"icon":"save",
													"action":"/config",
													"trigger":"config.wifi",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"t6 Object",
										"icon":"devices",
										"body":{
											"sections":[
												{
													"title":"Object",
													"width":12,
													"id":"object_section",
													"content": {
														"inputs":[
															{
																"label":"t6 Object identifier",
																"icon":"wifi",
																"placeholder":"object_id",
																"pattern": "(^[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}$)",
																"error":"Must be a uuid-v4",
																"width":12,
																"expandable":false,
																"id":"config.t6.t6Object_id",
																"class":""
															},
															{
																"label":"t6ObjectSecretKey",
																"icon":"password",
																"placeholder":"t6ObjectSecretKey",
																"pattern": "(^[a-fA-F0-9]{64}$)",
																"error":"Must be a 64 chars hexadecimal",
																"width":12,
																"expandable":false,
																"id":"config.t6.t6ObjectSecretKey",
																"class":""
															},
															{
																"label":"t6 scheme",
																"icon":"",
																"placeholder":"scheme",
																"pattern": "(^https?://$)",
																"error":"Must be http:// or https://",
																"width":12,
																"expandable":false,
																"id":"config.t6.scheme",
																"class":""
															},
															{
																"label":"t6 host",
																"icon":"",
																"placeholder":"host",
																"pattern": "",
																"error":"Must be ",
																"width":12,
																"expandable":false,
																"id":"config.t6.host",
																"class":""
															},
															{
																"label":"t6 port",
																"icon":"",
																"placeholder":"port",
																"pattern": "",
																"error":"Must be http:// or https://",
																"width":12,
																"expandable":false,
																"id":"config.t6.port",
																"class":""
															}
														],
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"Http",
										"icon":"dns",
										"body":{
											"sections":[
												{
													"title":"Service Status",
													"width":2,
													"id":"Http_section",
													"content": {
														"switche": {
															"id":"config.t6.servicesStatus.http",
															"valueUnchecked":0,
															"labelUnchecked":"Disabled",
															"valueChecked":1,
															"labelChecked":"Enabled",
															"defaultState":"checked",
															"action":""
														}
													}
												},
												{
													"title":"Http",
													"width":12,
													"id":"http_section",
													"content": {
														"inputs":[
															{
																"label":"http port",
																"icon":"nat",
																"placeholder":"port",
																"pattern": "([^\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.http.localPort",
																"class":""
															},
														],
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6.http",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6.http",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"Audio",
										"icon":"volume_up",
										"body":{
											"sections":[
												{
													"title":"Service Status",
													"width":2,
													"id":"Audio_section",
													"content": {
														"switche": {
															"id":"config.t6.servicesStatus.audio",
															"valueUnchecked":0,
															"labelUnchecked":"Disabled",
															"valueChecked":1,
															"labelChecked":"Enabled",
															"defaultState":"checked",
															"action":""
														}
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6.audio",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6.audio",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"Mdns",
										"icon":"dns",
										"body":{
											"sections":[
												{
													"title":"Service Status",
													"width":2,
													"id":"Mdns_section",
													"content": {
														"switche": {
															"id":"config.t6.servicesStatus.mdns",
															"valueUnchecked":0,
															"labelUnchecked":"Disabled",
															"valueChecked":1,
															"labelChecked":"Enabled",
															"defaultState":"checked",
															"action":""
														}
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6.mdns",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6.mdns",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"SSDP",
										"icon":"dns",
										"body":{
											"sections":[
												{
													"title":"Service Status",
													"width":2,
													"id":"Ssdp_section",
													"content": {
														"switche": {
															"id":"config.t6.servicesStatus.ssdp",
															"valueUnchecked":0,
															"labelUnchecked":"Disabled",
															"valueChecked":1,
															"labelChecked":"Enabled",
															"defaultState":"checked",
															"action":""
														}
													}
												},
												{
													"title":"SSDP",
													"width":12,
													"id":"Ssdp_section",
													"content": {
														"inputs":[
															{
																"label":"SSDP port",
																"placeholder":"port",
																"icon":"nat",
																"pattern": "^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.localPort",
																"class":""
															},
															{
																"label":"Advertise Interval",
																"placeholder":"advertiseInterval",
																"icon":"update",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.advertiseInterval",
																"class":""
															},
															{
																"label":"Presentation URL",
																"placeholder":"Presentation URL",
																"icon":"link",
																"pattern": "https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()!@:%_\+.~#?&\/\/=]*)",
																"error":"Must be a valid URL",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.presentationURL",
																"class":""
															},
															{
																"label":"friendlyName",
																"placeholder":"friendlyName",
																"icon":"android",
																"pattern": "([\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.friendlyName",
																"class":""
															},
															{
																"label":"modelName",
																"placeholder":"modelName",
																"icon":"badge",
																"pattern": "([\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.modelName",
																"class":""
															},
															{
																"label":"modelNumber",
																"placeholder":"modelNumber",
																"icon":"pin",
																"pattern": "([\d\.]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.modelNumber",
																"class":""
															},
															{
																"label":"deviceType",
																"placeholder":"deviceType",
																"icon":"keyboard_alt",
																"pattern": "([\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.deviceType",
																"class":""
															},
															{
																"label":"modelURL",
																"placeholder":"modelURL",
																"icon":"link",
																"pattern": "https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()!@:%_\+.~#?&\/\/=]*)",
																"error":"Must be a valid URL",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.modelURL",
																"class":""
															},
															{
																"label":"manufacturer",
																"placeholder":"manufacturer",
																"icon":"business",
																"pattern": "([^\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.manufacturer",
																"class":""
															},
															{
																"label":"manufacturerURL",
																"placeholder":"manufacturerURL",
																"icon":"link",
																"pattern": "https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()!@:%_\+.~#?&\/\/=]*)",
																"error":"Must be a valid URL",
																"width":12,
																"expandable":false,
																"id":"config.t6.ssdp.manufacturerURL",
																"class":""
															}
														]
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6.ssdp",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6.ssdp",
													"method":"PUT",
													"class":"mdl-button--raised"
												},
												{
													"label":"Description",
													"value":"",
													"icon":"record_voice_over",
													"action":"/description.xml",
													"trigger":"config.t6.ssdp",
													"method":"GET",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":12,
										"title":"Object WebSockets settings",
										"icon":"power",
										"body":{
											"sections":[
												{
													"title":"Service Status",
													"width":2,
													"id":"Sockets_section",
													"content": {
														"switche": {
															"id":"config.t6.servicesStatus.sockets",
															"valueUnchecked":0,
															"labelUnchecked":"Disabled",
															"valueChecked":1,
															"labelChecked":"Enabled",
															"defaultState":"checked",
															"action":""
														}
													}
												},
												{
													"title":"Web Sockets Server",
													"width":12,
													"id":"outlet",
													"content": {
														"inputs":[
															{
																"label":"Sockets server Host",
																"icon":"link",
																"placeholder":"",
																"pattern": "([^\w]+$)",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.host",
																"class":""
															},
															{
																"label":"Sockets server Port",
																"icon":"nat",
																"placeholder":"",
																"pattern": "^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.port",
																"class":""
															},
															{
																"label":"Sockets server Path",
																"icon":"power",
																"placeholder":"",
																"pattern": "^(.*?)",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.path",
																"class":""
															},
															{
																"label":"Auth Key",
																"icon":"vpn_key",
																"placeholder":"",
																"pattern": "^/(.*?)",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.t6wsKey",
																"class":""
															},
															{
																"label":"Auth Secret",
																"icon":"password",
																"placeholder":"",
																"pattern": "^(.*?)",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.t6wsSecret",
																"class":""
															}
														],
													}
												},
												{
													"title":"Web Sockets parameters",
													"width":12,
													"id":"outlet",
													"content": {
														"inputs":[
															{
																"label":"Message Interval",
																"icon":"av_timer",
																"placeholder":"Message Interval",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.messageInterval",
																"class":""
															},
															{
																"label":"Message Interval Once Claimed",
																"icon":"av_timer",
																"placeholder":"messageIntervalOnceClaimed",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.messageIntervalOnceClaimed",
																"class":""
															},
															{
																"label":"Reconnect Interval",
																"icon":"restore",
																"placeholder":"reconnectInterval",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.reconnectInterval",
																"class":""
															},
															{
																"label":"Timeout Interval",
																"icon":"hourglass_bottom",
																"placeholder":"timeoutInterval",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.timeoutInterval",
																"class":""
															},
															{
																"label":"disconnect After Failure",
																"icon":"running_with_errors",
																"placeholder":"disconnectAfterFailure",
																"pattern": "([\\d]+$)",
																"error":"Must be an integer",
																"width":12,
																"expandable":false,
																"id":"config.t6.websockets.disconnectAfterFailure",
																"class":""
															},
														],
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Save",
													"id":"config.t6.websockets",
													"icon":"save",
													"action":"/config",
													"trigger":"config.t6.websockets",
													"method":"PUT",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":6,
										"title":"Web Sockets Channel Subscription",
										"icon":"wifi",
										"body":{
											"sections":[
												{
													"title":"Channel Subscription",
													"width":12,
													"id":"Channel_Subscription_section",
													"content": {
														"inputs":[
															{
																"label":"Channel Name",
																"placeholder":"Channel Name",
																"icon":"wifi",
																"pattern": "([^ ]+)",
																"error":"Can't have a space",
																"width":12,
																"expandable":false,
																"id":"subscribe",
																"class":""
															}
														]
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Subscribe",
													"value":"",
													"icon":"bookmark_add",
													"action":"/subscribe?channel=%s",
													"trigger":"subscribe",
													"class":"mdl-button--raised"
												}
											]
										}
									},
									{
										"width":6,
										"title":"Web Sockets Channel Unsubscription",
										"icon":"wifi",
										"body":{
											"sections":[
												{
													"title":"Channel Unsubscription",
													"width":12,
													"id":"Channel_Unsubscription_section",
													"content": {
														"inputs":[
															{
																"label":"Channel Name",
																"placeholder":"channel_name",
																"icon":"wifi",
																"pattern": "([^ ]+)",
																"error":"Can't have a space",
																"width":12,
																"expandable":false,
																"id":"unsubscribe",
																"class":""
															}
														]
													}
												}
											]
										},
										"actions": {
											"buttons": [
												{
													"label":"Unsubscribe",
													"value":"",
													"icon":"bookmark_remove",
													"action":"/unsubscribe?channel=%s",
													"trigger":"unsubscribe",
													"class":"mdl-button--raised"
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
		{
			"name": "Help",
			"id": "help",
			"class": "",
			"icon": "contact_support",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab8_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab8_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"How to use Arduino Universal Controller?",
										"icon": "help",
										"body":{
											"texts":[
												{
													"text":"You first need to upload the sample generic Arduino source code to your device after editing the wifi credentials and t6 IoT Object settings.",
													"width":12,
													"id":"helptext1",
													"icon":"help",
													"class":""
												},
												{
													"text":"SSDP : gssdp-discover -i eth0 -r 5",
													"width":12,
													"id":"helptext2",
													"icon":"lightbulb",
													"class":""
												},
												{
													"text":"MDSN : mdns-scan",
													"width":12,
													"id":"helptext2",
													"icon":"lightbulb",
													"class":""
												}
											]
										}
									}
								]
							}
						]
					}
				]
			}
		},
	],
	"footer": {
		"sections": [
			{
				"title": "Features",
				"links": [
					{
						"label": "t6 IoT",
						"href": "https://api.internetcollaboratif.info/"
					},
					{
						"label": "Features",
						"href": "https://www.internetcollaboratif.info/features"
					},
					{
						"label": "Articles",
						"href": "https://www.internetcollaboratif.info/post/"
					}
				]
			},
			{
				"title": "Technical details",
				"links": [
					{
						"label": "Github repository",
						"href": "https://github.com/mathcoll/t6"
					},
					{
						"label": "t6 Api doc",
						"href": "https://doc.internetcollaboratif.info/"
					},
					{
						"label": "Install IoT platform",
						"href": "https://github.com/mathcoll/t6/wiki/Installing-t6"
					},
					{
						"label": "t6 iot Arduino library",
						"href": "https://github.com/mathcoll/t6iot"
					}
				]
			}
		]
	}
};