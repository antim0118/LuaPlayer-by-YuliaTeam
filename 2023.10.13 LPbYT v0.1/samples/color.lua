local color

while true do
	screen.clear()
	pad = buttons.read()
	
	intraFont.size(deFfont, 0.9)
	print(240-intraFont.textW(deFfont, lines[lang][1])/2,50,colors.white,lines[lang][1])
	print(240-intraFont.textW(deFfont, lines[lang][#lines[lang]])/2,200,colors.white,lines[lang][#lines[lang]])
	
	if color then
		intraFont.printShadowed(deFfont, 240-intraFont.textW(deFfont, 'r = '..Color.get(color).r..', g = '..Color.get(color).g..', b = '..Color.get(color).b..', a = '..Color.get(color).a)/2,132,'r = '..Color.get(color).r..', g = '..Color.get(color).g..', b = '..Color.get(color).b..', a = '..Color.get(color).a,color,colors.white,35,2)
	end
	
	if press('cross') then color = Color.new(LUA.getRandom(255),LUA.getRandom(255*2)/2,LUA.getRandom(255*4)/4) end
	if press('start') then break; end
	
	oldpad = pad
	screen.flip()
end

color = nil
-- clear the RAM
System.GC()
sound.play('assets/bg.mp3', sound.MP3, false, true)