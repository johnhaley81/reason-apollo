/* open Apollo.Links; */
open Apollo.InMemoryCache;

/* Create an InMemoryCache */
let inMemoryCache = createInMemoryCache();

/*
   OR with dataIdFromObject:

   type dataObject = {
     .
     "__typename": string,
     "id": string
   };

   createInMemoryCache(~dataIdFromObject=(obj: dataObject) => obj##id, ());
 */

/* Create a Link that puts an Authorization header in context */
let headerContextLink =
  Apollo.LinkContext.make(() =>
    {
      "headers": {
        "authorization": "Bearer $123",
      },
    }
  );

/* Create an HTTP Link */
let httpLink =
  Apollo.LinkHttp.make(
    ~uri="https://api.graph.cool/simple/v1/cjdgba1jw4ggk0185ig4bhpsn",
    (),
  );

/* WebSocket client */
let webSocketLink =
  Apollo.LinkWebSocket.make(
    ~uri="ws://localhost:8080/graphql",
    ~reconnect=true,
    (),
  );

/* based on test, execute left or right */
let webSocketHttpLink =
  Apollo.Link.split(
    operation => {
      let operationDefition =
        Apollo.Utilities.getMainDefinition(operation##query);
      operationDefition##kind == "OperationDefinition"
      &&
      operationDefition##operation == "subscription";
    },
    webSocketLink,
    httpLink,
  );

let instance =
  Apollo.Client.make(
    ~link=Apollo.Link.from([|webSocketHttpLink, headerContextLink|]),
    ~cache=inMemoryCache,
    (),
  );
