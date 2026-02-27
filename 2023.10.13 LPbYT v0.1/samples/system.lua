while true do
	screen.clear()
	pad = buttons.read()
	
	print(240-intraFont.textW(deFfont, lines[lang][#lines[lang]])/2,200,colors.white,lines[lang][#lines[lang]])
	
	print(10,10,colors.white,'free RAM: '..LUA.freeRam()..' kb')
	print(10,25,colors.white,'Nickname: '..System.getNickname())
	print(10,40,colors.white,'Language: '..System.getLang())
	print(10,55,colors.white,'Battery percentage: '..System.getBatteryPercent()..'%')
	print(10,70,colors.white,'Battery lifetime: '..System.getBatteryLifeTime()..' min')
	print(10,85,colors.white,'Model: '..System.getModel())
	print(10,100,colors.white,'CPU Speed: '..System.getCPU()..' MHz')
	print(10,115,colors.white,'OS version: '..System.getOSV())
	
	print(10,145,colors.white,'Date: '..System.getTime().day..'/'..System.getTime().month..'/'..System.getTime().year)
	print(10,160,colors.white,'Time: '..System.getTime().hour..':'..System.getTime().minutes..':'..System.getTime().seconds..':'..System.getTime().microseconds)
	
	if press('start') then break; end
	
	oldpad = pad
	screen.flip()
end

sound.play('assets/bg.mp3', sound.MP3, false, true)