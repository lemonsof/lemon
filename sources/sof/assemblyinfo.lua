assembly = 
{
   -- the project name
   name = "sof";

   description = "sof runtimes";

   company = "yayanyang@gmail.com";

   -- the project guid
   guid = "{ 0x612c9e2e, 0x2341, 0x4a7d, { 0x8a, 0x12, 0x3c, 0x2e, 0x8e, 0x58, 0xc8, 0x53 } }";

   -- the project errorcode define
   errorcode=
   {
	   {
	   		name = "RESOURCE_ERROR";
	   };

	   {
	   		name = "NOT_IMPLEMENT";
	   };

	   {
			name = "EXTENSION_NOT_FOUND";
	   }
   };
}