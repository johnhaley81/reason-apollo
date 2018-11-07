/* Create an InMemoryCache */
let inMemoryCache = Apollo.Cache.InMemory.make();

/* Create an HTTP Link */
let httpLink =
  Apollo.Link.Http.make(
    ~uri="https://boiling-bastion-96890.herokuapp.com/graphql",
    (),
  );

/* WebSocket client */
let webSocketLink =
  Apollo.Link.WebSocket.make(
    ~uri="wss://boiling-bastion-96890.herokuapp.com/graphql",
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
  Apollo.Client.make(~link=webSocketHttpLink, ~cache=inMemoryCache, ());
