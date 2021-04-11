
function init(id)
	mylib.print_message("in init. got: " .. id)
	local announce_id = mylib.announce("henry")
	mylib.print_message("announce got " .. announce_id)
end

function shutdown()
	mylib.print_message ("in shutdown")
end
