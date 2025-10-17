local bool = function(arg) return not (arg == "" or arg == nil or arg == false or arg == 0) end

asyncCycle = {
	array = {},
	
	upd = function(async)
		if timer.time(async.time) > async.ms then
			async.i = async.i + async.step
			timer.reset(async.time)
			timer.start(async.time)
		end
		
		if (async.step > 0 and async.i > async.max) or (async.step < 0 and async.i < async.max) then
			async.i = async.max
		end
		
		async.func(async.i)
	end,
	
	update = function(self)
		if not bool(self) then self = asyncCycle end
		
		if #self.array > 0 then
			for i = 1, #self.array do
				if self.array[i] then
					self.upd(self.array[i])
					if self.array[i].i == self.array[i].max then
						timer.remove(self.array[i].time)
						table.remove(self.array, i)
					end
				end
			end
		end
	end,
	
	new = function(self, from, to, step, ms, func)
		if not bool(self) then self = asyncCycle end
		
		local o = {
			ms = ms,
			time = timer.create(),
			
			i = from,
			max = to,
			step = step,
			func = func,
		}
		
		table.insert(self.array, o)
	end
}