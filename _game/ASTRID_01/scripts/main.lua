loadSprite("Milk", "milk.png");

local player = entity({
    sprite("Milk"),
    position(0, 0),
    scale(2)
});

player:OnUpdate(function(self, dt)

    self.x = self.x + 60 * dt

end)

-- scene("menu", require("../scenes.menu"))

-- go("menu")