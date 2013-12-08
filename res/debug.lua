filthy = Crew {
	update = function(self)
		print("I was made in Lua")
		return "PAUSE"
	end
}

function filthy:animate()
	puts("crazee")
end

cuetest = Crew {
	update = function(self)
		if persist and emit then 
			persist("hello, world")
			emit { tag = "lolemit", racist = "yes"  }
			emit { notag = true  }
			if cue "hello, world" then print("hello world is emitted") end
			if cue "lolemit" then print("emitted nil") end
			return "CUT"
		end
		return "CUT"
	end
}

--[[myke = Actor {
	animate = "LOL"
}]]


local RIGHT = 0
local LEFT = 1

local BOIL_REEL = 0
local RUN_REEL = 2

daisy = Actor {
	true,"lol wut",1,2,3,4,5,
	tag = "daisy chane",
	costume = "daisy-flat.png",
	w = 100,
	h = 100,
	ticks = 0,
	frame = 1,
	reel = 0,
	visible = true,
	x = 800,
	y = 200,
	vector = 10,
	facing = RIGHT,
	nest = {murgh = "yes",yarp = "no"},
	troo = true,
	faux = false,
	array = {1, 2, 3, 4, 5},6,7,8
}

brum = Actor {
	tag = "butthead",
	costume = "brum.png",
	visible = true,
	w = 200,
	h = 200,
	x = 0,
	y = 0
}

function brum:animate()
	if self.reel ~= 2 then self:jumpreel(2) end
	if self.ticks == 20 then
		if self.frame == 1 then self:nextclip() 
		elseif self.frame == 2 then
			self:prevclip()
			self.frame = 1
		end
	end
	return self.animate
end

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
	local left = cue("PLAYER_LEFT")
	local right = cue("PLAYER_RIGHT")

	if (left and not right) or (not left and right) then return self.run end
	if self.reel ~= (BOIL_REEL + self.facing) then self:jumpreel(BOIL_REEL + self.facing) end

	if self.ticks == 3 then
		if (is_even(self.frame)) then self:prevclip()
		else self:nextclip() end
	end

	self:focus()
	return self.boil
end

-- jumpreel should take care of handling the resetting of the values frame, clip and reel
-- perhaps I should call it setreel? Shorter... though the word jumpreel looks more... right!

function daisy:run()
	local left = cue("PLAYER_LEFT")
	local right = cue("PLAYER_RIGHT")

	if(left and right) or (not left and not right) then return self.boil
	elseif left then 
		self.vector = -20
		self.facing = LEFT
	elseif right then 
		self.vector = 20
		self.facing = RIGHT
	end

	if self.reel ~= (RUN_REEL + self.facing) then self:jumpreel(RUN_REEL + self.facing) end

	if self.ticks == 2 then
		if self.frame == 1 
		or self.frame == 2 then
			self:nextclip()
		elseif self.frame == 3
		or self.frame == 4 then
			self:prevclip()
		end

		if self.frame == 5 then 
			self.frame = 1 
		end
	end
	self:focus()
	self.x = self.x + self.vector

	return self.run	
end

serialize(daisy)
print "\n"
serialize(daisy)
