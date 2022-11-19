let ui = {
	"title": "Arduino Universal Controler",
	"header": {
		"class": "mdl-layout--fixed-header",
		"drawer": {
			"title": "Arduino Universal Controler",
			"links": [
				{"name": "t6 IoT App", "icon": "api", "id": "t6iot", "class": "", "link": "https://api.internetcollaboratif.info"},
				{"name": "t6 Api doc", "icon": "integration_instructions", "id": "apidoc", "class": "", "link": "https://doc.internetcollaboratif.info"},
				{"spacer": true},
				{"name": "Digital", "icon": "pin_invoke", "id": "digitalMenu", "class": "", "link": "#digital"},
				{"name": "Analog", "icon": "pin_invoke", "id": "analogMenu", "class": "", "link": "#analog"},
				{"name": "Led", "icon": "wb_incandescent", "id": "ledMenu", "class": "", "link": "#led"},
				{"name": "Audio", "icon": "volume_up", "id": "audioMenu", "class": "", "link": "#audio"},
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
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin0",
															"label":"Digital Pin 0",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=0&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin1",
															"label":"Digital Pin 1",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=1&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin2",
															"label":"Digital Pin 2",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=2&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin3",
															"label":"Digital Pin 3",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=3&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin4",
															"label":"Digital Pin 4",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=4&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin5",
															"label":"Digital Pin 5",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=5&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin6",
															"label":"Digital Pin 6",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=6&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin7",
															"label":"Digital Pin 7",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=7&value=%s"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"",
													"switches":[
														{
															"id":"pin8",
															"label":"Digital Pin 8",
															"valueUnchecked":0,
															"labelUnchecked":"LOW",
															"valueChecked":1,
															"labelChecked":"HIGH",
															"defaultState":"unchecked",
															"action":"/digitalWrite?pin=8&value=%s"
														}
													]
												}
											]
										},
										"actions":{}
									},
									{
										"width":6,
										"title":"digital Read",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD0",
													"buttons": [
														{
															"id":"pinD0",
															"label":"Read pin 0",
															"action":"/digitalRead?pin=0",
															"trigger":"trigger_pinD0",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD1",
													"buttons": [
														{
															"id":"pinD1",
															"label":"Read pin 1",
															"action":"/digitalRead?pin=1",
															"trigger":"trigger_pinD1",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD2",
													"buttons": [
														{
															"id":"pinD2",
															"label":"Read pin 2",
															"action":"/digitalRead?pin=2",
															"trigger":"trigger_pinD2",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD3",
													"buttons": [
														{
															"id":"pinD3",
															"label":"Read pin 3",
															"action":"/digitalRead?pin=3",
															"trigger":"trigger_pinD3",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD4",
													"buttons": [
														{
															"id":"pinD4",
															"label":"Read pin 4",
															"action":"/digitalRead?pin=4",
															"trigger":"trigger_pinD4",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD5",
													"buttons": [
														{
															"id":"pinD5",
															"label":"Read pin 5",
															"action":"/digitalRead?pin=5",
															"trigger":"trigger_pinD5",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD6",
													"buttons": [
														{
															"id":"pinD6",
															"label":"Read pin 6",
															"action":"/digitalRead?pin=6",
															"trigger":"trigger_pinD6",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD7",
													"buttons": [
														{
															"id":"pinD7",
															"label":"Read pin 7",
															"action":"/digitalRead?pin=7",
															"trigger":"trigger_pinD7",
															"class":"mdl-button--raised"
														}
													]
												},
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"trigger_pinD8",
													"buttons": [
														{
															"id":"pinD8",
															"label":"Read pin 8",
															"action":"/digitalRead?pin=8",
															"trigger":"trigger_pinD8",
															"class":"mdl-button--raised"
														}
													]
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
			"icon": "pin_invoke",
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
										"body":{
											"slider":
												{
													"label":"A0",
													"id":"sliderA0",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":1,
													"action":"/analogWrite?pin=0&value=%s"
												}
										},
										"actions":{}
									},
									{
										"width":8,
										"title":"analogRead",
										"body":{
											"lists": [
												{
													"icon": "pin_invoke",
													"label":"n/a",
													"label_id":"pinD0",
													"buttons": [
														{
															"label":"analogRead 0",
															"action":"/analogRead?pin=0",
															"trigger":"pinA0",
															"class":"mdl-button--raised"
														}
													]
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
													"step":1,
													"action":"/analogWrite?pin=5&value=%s"
												},
												{
													"label":"Green",
													"id":"sliderGreen",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":1,
													"action":"/analogWrite?pin=4&value=%s"
												},
												{
													"label":"Blue",
													"id":"sliderBlue",
													"width":12,
													"min":0,
													"max":255,
													"value":0,
													"step":1,
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
										"body":{
											"inputs":[
												{
													"text":"Text message:",
													"width":12,
													"id":"audioOutput",
													"class":""
												}
											],
											"buttons":[
												{
													"label":"Say",
													"value":"",
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
			"name": "Help",
			"id": "help",
			"class": "",
			"icon": "contact_support",
			"width":12,
			"body": {
				"rows":[
					{
						"row_id": "tab5_row1",
						"width":12,
						"columns":[
							{
								"width":12,
								"col_id":"tab5_row1_col1",
								"cards":[
									{
										"width":12,
										"title":"How to use Arduino Universal Controller?",
										"body":{
											"texts":[
												{
													"text":"You first need to upload the sample generic Arduino source code to your device after editing the wifi credentials and t6 IoT Object settings.",
													"width":12,
													"id":"helptext",
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
		}
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