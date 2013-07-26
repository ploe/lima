filthy = Crew {
	update = function(self)
		print("I was made in Lua")
		return "PAUSE"
	end
}

function filthy:animate()
	puts("crazee")
end

--[[myke = Actor {
	animate = "LOL"
}]]

daisy = Actor {
	tag = "daisy chane",
	costume = "daisy-flat.png",
	w = 100,
	h = 100
}

print(daisy.costume)
print(daisy.w)
print(daisy.h)
print(daisy.tag)

daisy.frame = 0

function daisy:animate()
	print(self)
	return self.tick
end

function daisy:tock()
	if self.frame < 3 then 
		self.frame = self.frame + 1
		return self.tock
	end

	self.frame = 0
	prevclip(self)
	return self.tick
end

function daisy:tick()
	jumpreel(self)
	if self.frame < 3 then 
		self.frame = self.frame + 1
		return self.tick
	end

	self.frame = 0
	nextclip(self)
	return self.tock
end

