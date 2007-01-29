function lua_test( i , o , s )
  cpp_test( i , o )
  cpp_test_2( 2.0 , 3.0, "bulka" )
  print( s )
  
  return i + o * 3
end


function lua_test_2( )
  print( "babuka" )
  
  g = add_vec(5, 6)
  
  print_f(g)
  
  return g;
end