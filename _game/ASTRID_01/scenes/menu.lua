return function ()

    add({
        sprite("checker"),
        pos(0, 0),
        scale(2),
        layer("bg")
    })

    local title = add({
        sprite("game"),
        pos(100, 100),
        scale(2),
        anchor("center"),
        layer("ui")
    })

    title:onUpdate(function ()
        title.rotation = math.sin(0.3)
    end)

end