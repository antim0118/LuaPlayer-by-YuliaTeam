function bool(arg) return not (arg == "" or arg == nil or arg == false or arg == 0) end

asyncCycle = {
	array = {},
	time = timer.create(),
	
	upd = function(self, async, currentTime)
		if currentTime == 0 then
			timer.reset(self.time)
			timer.start(self.time)
		elseif currentTime > async.ms then
			async.i = async.i + async.step
			timer.reset(self.time)
			timer.start(self.time)
		end
		
		if (async.step > 0 and async.i > async.max) or (async.step < 0 and async.i < async.max) then
			async.i = async.max
		end
		
		async.func(async.i)
	end,
	
	update = function(self)
		if not bool(self) then self = asyncCycle end
		
		if #self.array > 0 then
			local currentTime = timer.time(self.time)
			
			for i = 1, #self.array do
				local async = self.array[i]
				if async then
					self:upd(async, currentTime)
					if async.i == async.max then
						table.remove(self.array, i)
					end
				end
			end
		elseif timer.time(self.time) > 0 then
			timer.reset(self.time)
		end
	end,
	
	new = function(self, from, to, step, ms, func)
		if not bool(self) then self = asyncCycle end
		
		table.insert(self.array, {ms = ms, i = from, max = to, step = step, func = func})
	end
}

timer.reset(asyncCycle.time)