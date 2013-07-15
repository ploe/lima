filthy = Crew {
	update = function(self)
		print("I was made in Lua")
		return "PAUSE"
	end
}

function filthy:animate()
	puts("crazee")
end

myke = Actor {
	animate = "LOL"
}

