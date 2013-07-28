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
	h = 100,
	ticks = 0,
	frame = 1,
	reel = 0
}

print(daisy.costume)
print(daisy.w)
print(daisy.h)
print(daisy.tag)


function daisy:animate()
	return self.boil	
end

-- Don't want to divide by zero...
function is_even(n)
	if (n ~= 0) and ((n % 2) == 0) then return true end
	return false
end

--[[ maybe self.ticks can be incremented in C, that way we don't have to
add it up in Lua which removes a little more of the boilerplate bullshit

nextclip, prevclip and jumpclip could all increment the frame counter 
which again removes a little more of the boilerplate stuff. ]]

function daisy:boil()
	if self.reel ~= 0 then jumpreel(0, self) end

	if self.ticks == 3 then
		if (is_even(self.frame)) then prevclip(self)
		else nextclip(self) end
	end

	return self.boil
end

-- jumpreel should take care of handling the resetting of the values frame, clip and reel
-- perhaps I should call it setreel? Shorter... though the word jumpreel looks more... right!

function daisy:run()
	if self.reel ~= 1 then jumpreel(1, self) end

	if self.ticks == 3 then
		if self.frame == 1 
		or self.frame == 2 then
			nextclip(self)
		elseif self.frame == 3
		or self.frame == 4 then
			prevclip(self)
		end

		if self.frame > 4 then self.frame = 1 end
	end
	
	return self.run	
end
