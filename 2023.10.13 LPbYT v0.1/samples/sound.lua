-- read all the files from sound folders
local music = {
	mp3_list = System.listDir('assets/mp3'),
	wav_list = System.listDir('assets/wav'),
	volume = {wav = 100, mp3 = 100},
}

-- remove unused fields
table.remove(music.mp3_list,1)
table.remove(music.wav_list,1)

local column, num, color = 'mp3', 1

while true do
	screen.clear()
	pad = buttons.read()
	
	for i, v in ipairs(music.mp3_list) do
		if num == i then color = Color.new(76, 102, 240) column = 'mp3' else color = colors.white end
		print(10,20+i*12,color,v.name)
	end
	
	print(240,32,colors.white,'is MP3 channel free: '..tostring(sound.state(sound.MP3).free))
	print(240,160,colors.white,'is WAV channel free: '..tostring(sound.state(sound.WAV_1).free))
	print(240,44,colors.white,'ID3 tags: \n\n'..'title: '..sound.info().title..'\nartist: '..sound.info().artist..'\nalbum: '..sound.info().album..'\nyear: '..sound.info().year..'\ngenre: '..sound.info().genre..'\nidv3: '..sound.info().id3v..'\ntrack: '..tostring(sound.info().track))
	print(240,180,colors.white,tostring('MP3 volume: '..music.volume.mp3..'\n'..'WAV volume: '..music.volume.wav))
	print(240-intraFont.textW(deFfont, lines[lang][2])/2, 225,colors.white,lines[lang][2])
	print(240-intraFont.textW(deFfont, lines[lang][3])/2, 240,colors.white,lines[lang][3])
	print(240-intraFont.textW(deFfont, lines[lang][#lines[lang]])/2,255,colors.white,lines[lang][#lines[lang]])
	
	for i, v in ipairs(music.wav_list) do
		if num == i+#music.mp3_list then color = Color.new(76, 102, 240) column = 'wav' else color = colors.white end
		print(10,120+i*12,color,v.name)
	end
	
	if press('cross') then
		if column == 'wav' then
			for i = 1, 31 do
				if sound.state(sound['WAV_'..tostring(i)]).state == "playing" then sound.stop(sound['WAV_'..tostring(i)]) end
			end
		end
		local path = 'assets/'..column..'/'..music[column..'_list'][(num <= #music.mp3_list and num) or num-#music.mp3_list].name
		local channel = sound[(column == 'wav' and column:upper()..'_1') or column:upper()]
		sound.play(path, channel, false, true)
	end
	
	if press('circle') then if column == 'mp3' then sound.stop(sound.MP3) else sound.stop(sound.WAV_1) end end
	if press('down') and num < #music.mp3_list + #music.wav_list then num = num + 1 end
	if press('up') and num > 1 then num = num - 1 end
	if press('left') and music.volume[column] > 0 then music.volume[column] = music.volume[column] - 10 sound.volume(sound[(column == 'wav' and column:upper()..'_1') or column:upper()], music.volume[column]) end 
	if press('right') and music.volume[column] < 100 then music.volume[column] = music.volume[column] + 10 sound.volume(sound[(column == 'wav' and column:upper()..'_1') or column:upper()], music.volume[column]) end 
	
	if press('start') then break; end
	
	oldpad = pad
	screen.flip()
end

if sound.state(sound.MP3).state == "playing" then sound.stop(sound.MP3) end

for i = 1, 31 do
	if sound.state(sound['WAV_'..tostring(i)]).state == "playing" then sound.stop(sound['WAV_'..tostring(i)]) end
end

sound.play('assets/bg.mp3', sound.MP3, false, true)