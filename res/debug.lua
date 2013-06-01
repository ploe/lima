Crew {
	update = function(self)
		print("I was made in Lua")
		return "PAUSE"
	end
}


Crew {
	count = 10,
	update = function(self)
		print(self.count)
		if self.count == 0 then return "WRAP" end
		self.count = self.count - 1
		return "LIVE"
	end
}
