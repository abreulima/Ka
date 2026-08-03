return function ()

    local player = add({
        sprite("Slime"),
        pos(300, 300),
        scale(2),
        area(),
        "player"
    })

    player:onUpdate(function(dt)

        if isKeyDown("left") then
            player:flipX(true)
            player:move(-200, 0)
        end

        if isKeyDown("right") then
            player:flipX(false)
            player:move(200, 0)
        end
        
    end)

end