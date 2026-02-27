local timered = timer.create()
timer.stop(timered)

while true do
	screen.clear()
	pad = buttons.read()
	
	print(240-intraFont.textW(deFfont, lines[lang][#lines[lang]])/2,200,colors.white,lines[lang][#lines[lang]])
	
	print(10,10,colors.white,lines[lang][4])
	print(10,25,colors.white,lines[lang][5])
	print(10,40,colors.white,'Timer time: '..tostring(timer.time(timered)))
	
	if press('cross') then timer.start(timered) end
	if press('circle') then timer.stop(timered) end
	if press('triangle') then timer.reset(timered) end
	
	if press('start') then break; end
	
	oldpad = pad
	screen.flip()
end

timer.stop(timered)
timer.remove(timered)
sound.play('assets/bg.mp3', sound.MP3, false, true)