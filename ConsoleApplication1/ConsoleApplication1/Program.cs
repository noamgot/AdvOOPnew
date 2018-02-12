using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            var telemetryDataPoint = new
            {
                messageId = 6,
                deviceId = 56456,
                temperature = 47,
                humidity = 53
            };
      
            var messageString = JsonConvert.SerializeObject(telemetryDataPoint);
            JObject jstring = JObject.Parse(messageString);
            var sum = jstring.GetValue("humidity");
            //var jobj = JsonConvert.DeserializeObject(messageString);
        }
    }
}
