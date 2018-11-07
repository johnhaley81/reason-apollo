/**
 * An abstract type to describe a query string object.
 */
type queryString;

/**
 * The signature of the `graphql-tag/gql` function that transforms a GraphQL
 * query string to the standard GraphQL AST.
 * https://github.com/apollographql/graphql-tag
 */
type gql = (. string) => queryString;

[@bs.module] external gql: gql = "graphql-tag";

/**
 * An abstract type to describe an Apollo Link object.
 */
type apolloLink;

/**
 * An abstract type to describe an Apollo Cache object.
 */
type apolloCache;

type networkError = {. "statusCode": int};

type graphqlError = {
  .
  "message": string,
  "locations": Js.Nullable.t(array(string)),
  "path": Js.Nullable.t(array(string)),
  "nodes": Js.Nullable.t(array(string)),
};

type executionResult = {
  .
  "errors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "data": Js.Nullable.t(Js.Json.t),
};

/* TODO define all types */
type operation = {. "query": queryString};

/* TODO define subscription */
type subscription;

type errorResponse = {
  .
  "graphQLErrors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "networkError": Js.Nullable.t(networkError),
  "response": Js.Nullable.t(executionResult),
  "operation": operation,
  "forward": operation => subscription,
};

module type Config = {
  let query: string;
  type t;
  let parse: Js.Json.t => t;
};

type apolloError = {
  .
  "message": string,
  "graphQLErrors": Js.Nullable.t(array(string)),
  "networkError": Js.Nullable.t(string),
};

type apolloOptions = {
  .
  "query": queryString,
  "variables": Js.Json.t,
};

type queryResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a);

type mutationResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a)
  | NotCalled;

type subscriptionResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a);

/*
 apollo link ws
 */

[@bs.deriving abstract]
type webSocketLinkOptionsT = {
  [@bs.optional]
  reconnect: bool,
};

[@bs.deriving abstract]
type webSocketLinkT = {
  uri: string,
  options: webSocketLinkOptionsT,
};

type documentNodeT;

type splitTest = {. "query": documentNodeT};

type queryObj = {
  .
  "query": queryString,
  "variables": Js.Json.t,
};

type mutationObj = {
  .
  "mutation": queryString,
  "variables": Js.Json.t,
};

type onErrorT;

[@bs.deriving abstract]
type updateQueryOptions = {
  [@bs.optional]
  fetchMoreResult: Js.Json.t,
  [@bs.optional]
  variables: Js.Json.t,
};

type updateQueryT = (Js.Json.t, updateQueryOptions) => Js.Json.t;

type updateSubscriptionOptions = {
  .
  "subscriptionData": Js.Nullable.t(Js.Json.t),
  "variables": Js.Nullable.t(Js.Json.t),
};
type updateQuerySubscriptionT =
  (Js.Json.t, updateSubscriptionOptions) => Js.Json.t;

/* We dont accept a new query for now */
[@bs.deriving abstract]
type fetchMoreOptions = {
  [@bs.optional]
  variables: Js.Json.t,
  updateQuery: updateQueryT,
};

[@bs.deriving abstract]
type subscribeToMoreOptions = {
  document: queryString,
  [@bs.optional]
  variables: Js.Json.t,
  [@bs.optional]
  updateQuery: updateQuerySubscriptionT,
  [@bs.optional]
  onError: onErrorT,
};

[@bs.deriving abstract]
type queryRenderPropObjJS = {
  loading: bool,
  data: Js.Nullable.t(Js.Json.t),
  error: Js.Nullable.t(apolloError),
  refetch:
    Js.Null_undefined.t(Js.Json.t) => Js.Promise.t(queryRenderPropObjJS),
  networkStatus: int,
  variables: Js.Null_undefined.t(Js.Json.t),
  fetchMore: fetchMoreOptions => Js.Promise.t(unit),
  subscribeToMore: (subscribeToMoreOptions, unit) => unit,
};

type mutationRenderPropObjJS = {
  .
  "loading": bool,
  "called": bool,
  "data": Js.Nullable.t(Js.Json.t),
  "error": Js.Nullable.t(apolloError),
  "networkStatus": int,
  "variables": Js.Null_undefined.t(Js.Json.t),
};

type generatedApolloClient = {
  .
  "query": [@bs.meth] (queryObj => Js.Promise.t(queryRenderPropObjJS)),
  "mutate": [@bs.meth] (mutationObj => Js.Promise.t(mutationRenderPropObjJS)),
  "resetStore": [@bs.meth] (unit => unit),
};

module Utilities = {
  type operationDefinitionNode = {
    .
    "kind": string,
    "operation": string,
  };

  [@bs.module "apollo-utilities"]
  external getMainDefinition: documentNodeT => operationDefinitionNode =
    "getMainDefinition";

  let getNonEmptyObj = jsObj =>
    switch (jsObj |> Js.Nullable.toOption) {
    | None => None
    | Some(data) =>
      switch (Js.Json.decodeObject(data)) {
      | None => None
      | Some(data) =>
        switch (Array.length(Js.Dict.keys(data))) {
        | 0 => None
        | _ => Some(Js.Json.object_(data))
        }
      }
    };
};

module Client = {
  type fetch;

  type linkOptions = {
    .
    "uri": string,
    "includeExtensions": Js.Nullable.t(bool),
    "fetch": Js.Nullable.t(fetch),
    "headers": Js.Nullable.t(Js.Json.t),
    "credentials": Js.Nullable.t(string),
    "fetchOptions": Js.Nullable.t(Js.Json.t),
  };

  type uploadLinkOptions = {
    .
    "uri": Js.Nullable.t(string),
    "fetch": Js.Nullable.t(fetch),
    "fetchOptions": Js.Nullable.t(Js.t({.})),
    "credentials": Js.Nullable.t(string),
    "headers": Js.Nullable.t(Js.Json.t),
    "includeExtensions": Js.Nullable.t(bool),
  };

  [@bs.module "apollo-client"] [@bs.new]
  external createApolloClientJS: 'a => generatedApolloClient = "ApolloClient";

  [@bs.obj]
  external apolloClientObjectParam:
    (
      ~link: apolloLink,
      ~cache: apolloCache,
      ~ssrMode: bool=?,
      ~ssrForceFetchDelay: int=?,
      ~connectToDevTools: bool=?,
      ~queryDeduplication: bool=?
    ) =>
    _ =
    "";

  /*
   * Expose a make function to create a Client that has to be passed to the ApolloProvider
   */
  let make =
      (
        ~link,
        ~cache,
        ~ssrMode=?,
        ~ssrForceFetchDelay=?,
        ~connectToDevTools=?,
        ~queryDeduplication=?,
        (),
      ) =>
    createApolloClientJS(
      apolloClientObjectParam(
        ~link,
        ~cache,
        ~ssrMode?,
        ~ssrForceFetchDelay?,
        ~connectToDevTools?,
        ~queryDeduplication?,
      ),
    );
};

module Link = {
  /* Bind the method `from`, used to compose links together */
  [@bs.module "apollo-link"]
  external from: array(apolloLink) => apolloLink = "from";

  /* Bind the method split. Based on a test send left or right */
  [@bs.module "apollo-link"]
  external split: (splitTest => bool, apolloLink, apolloLink) => apolloLink =
    "split";
};

module LinkContext = {
  /* Bind the setContext method */
  [@bs.module "apollo-link-context"]
  external make_: (unit => Js.t({..})) => apolloLink = "setContext";

  /**
 * CreateContextLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-context
 */
  let make = contextHandler =>
    /* Instanciate a new context link object */
    make_(contextHandler);
};

module LinkError = {
  [@bs.module "apollo-link-error"]
  external make_: (errorResponse => unit) => apolloLink = "onError";

  /**
 * CreateErrorLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-error
 */
  let make = errorHandler =>
    /* Instanciate a new error link object */
    make_(errorHandler);
};

module LinkHttp = {
  /* Bind the HttpLink class */
  [@bs.module "apollo-link-http"] [@bs.new]
  external make_: Client.linkOptions => apolloLink = "HttpLink";

  /**
 * CreateHttpLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-http
 */
  let make =
      (
        ~uri,
        ~includeExtensions=?,
        ~fetch=?,
        ~headers=?,
        ~credentials=?,
        ~fetchOptions=?,
        (),
      ) =>
    make_({
      "uri": uri,
      "includeExtensions": Js.Nullable.fromOption(includeExtensions),
      "fetch": Js.Nullable.fromOption(fetch),
      "headers": Js.Nullable.fromOption(headers),
      "credentials": Js.Nullable.fromOption(credentials),
      "fetchOptions": Js.Nullable.fromOption(fetchOptions),
    });
};

module LinkUpload = {
  /* Bind createUploadLink function from apollo upload link */
  [@bs.module "apollo-upload-client"]
  external make_: Client.uploadLinkOptions => apolloLink = "createUploadLink";

  /**
 * CreateUploadLink
 * https://github.com/jaydenseric/apollo-upload-client#function-createuploadlink
 */
  let make =
      (
        ~uri=?,
        ~fetch=?,
        ~fetchOptions=?,
        ~credentials=?,
        ~headers=?,
        ~includeExtensions=?,
        (),
      ) =>
    make_(
      Js.Nullable.{
        "uri": fromOption(uri),
        "fetch": fromOption(fetch),
        "fetchOptions": fromOption(fetchOptions),
        "credentials": fromOption(credentials),
        "headers": fromOption(headers),
        "includeExtensions": fromOption(includeExtensions),
      },
    );
};

module LinkWebSocket = {
  /* bind apollo-link-ws */
  [@bs.module "apollo-link-ws"] [@bs.new]
  external make_: webSocketLinkT => apolloLink = "WebSocketLink";

  let make = (~uri, ~reconnect=?, ()) =>
    make_(
      webSocketLinkT(~uri, ~options=webSocketLinkOptionsT(~reconnect?, ())),
    );
};

module InMemoryCache = {
  /**
 * Used on the client to rehydrate the cache using the initial data passed from the server
 * - e.g. window.__APOLLO_STATE__
 */
  type restoreData;

  /**
 * Define the data to pass to the restore method that'll be used used to rehydrate client.
 * If you don't want to pass any data, simply return `Js_null_undefined.undefined`.
 */
  type inMemoryCacheRestoreData = Js.Nullable.t(restoreData);

  /**
 * CreateInMemoryCache
 * https://github.com/apollographql/apollo-client/tree/master/packages/apollo-cache-inmemory
 */
  /* Bind the InMemoryCache class */
  [@bs.module "apollo-cache-inmemory"] [@bs.new]
  external apolloInMemoryCache: 'a => apolloCache = "InMemoryCache";

  /* Bind the restore method */
  [@bs.send.pipe: 't]
  external restore: inMemoryCacheRestoreData => apolloCache = "restore";

  /* Fragment matcher */
  type fragmentMatcher;

  [@bs.module "apollo-cache-inmemory"] [@bs.new]
  external introspectionFragmentMatcher: Js.t({..}) => fragmentMatcher =
    "IntrospectionFragmentMatcher";

  let createIntrospectionFragmentMatcher = (~data) =>
    introspectionFragmentMatcher({"introspectionQueryResultData": data});

  /* Instantiate a new cache object */
  [@bs.obj]
  external makeApolloInMemoryCacheParams:
    (
      ~dataIdFromObject: Js.t({..}) => string=?,
      ~fragmentMatcher: fragmentMatcher=?
    ) =>
    _ =
    "";

  let createInMemoryCache = (~dataIdFromObject=?, ~fragmentMatcher=?, ()) =>
    /* Apollo Client, looks for key in Object. Doesn't check if value is null  */
    apolloInMemoryCache(
      makeApolloInMemoryCacheParams(~dataIdFromObject?, ~fragmentMatcher?),
    );
};

module Provider = {
  [@bs.module "react-apollo"]
  external apolloProvider: ReasonReact.reactClass = "ApolloProvider";

  let make = (~client: generatedApolloClient, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=apolloProvider,
      ~props={"client": client},
      children,
    );
};

module Consumer = {
  [@bs.module "react-apollo"]
  external apolloConsumer: ReasonReact.reactClass = "ApolloConsumer";

  let make = (children: generatedApolloClient => ReasonReact.reactElement) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=apolloConsumer,
      ~props=Js.Obj.empty(),
      children,
    );
};

/*
 * Expose a module to perform "query" operations for the given client
 */
module CreateQuery = (Config: Config) => {
  [@bs.module "react-apollo"]
  external queryComponent: ReasonReact.reactClass = "Query";
  type response = queryResponse(Config.t);

  type renderPropObj = {
    result: response,
    data: option(Config.t),
    error: option(apolloError),
    loading: bool,
    refetch: option(Js.Json.t) => Js.Promise.t(response),
    fetchMore:
      (~variables: Js.Json.t=?, ~updateQuery: updateQueryT, unit) =>
      Js.Promise.t(unit),
    networkStatus: int,
    subscribeToMore:
      (
        ~document: queryString,
        ~variables: Js.Json.t=?,
        ~updateQuery: updateQuerySubscriptionT=?,
        ~onError: onErrorT=?,
        unit,
        unit
      ) =>
      unit,
  };

  let graphqlQueryAST = gql(. Config.query);
  let apolloDataToVariant: queryRenderPropObjJS => response =
    apolloData =>
      switch (
        apolloData->loadingGet,
        apolloData->dataGet |> Js.Nullable.toOption,
        apolloData->errorGet |> Js.Nullable.toOption,
      ) {
      | (true, _, _) => Loading
      | (false, Some(response), _) => Data(Config.parse(response))
      | (false, _, Some(error)) => Error(error)
      | (false, None, None) =>
        Error({
          "message": "No data",
          "graphQLErrors": Js.Nullable.null,
          "networkError": Js.Nullable.null,
        })
      };

  let convertJsInputToReason = (apolloData: queryRenderPropObjJS) => {
    result: apolloData |> apolloDataToVariant,
    data:
      switch (apolloData->dataGet |> Utilities.getNonEmptyObj) {
      | None => None
      | Some(data) =>
        switch (Config.parse(data)) {
        | parsedData => Some(parsedData)
        | exception _ => None
        }
      },
    error:
      switch (apolloData->errorGet |> Js.Nullable.toOption) {
      | Some(error) => Some(error)
      | None => None
      },
    loading: apolloData->loadingGet,
    refetch: variables =>
      apolloData->(refetchGet(variables |> Js.Nullable.fromOption))
      |> Js.Promise.then_(data =>
           data |> apolloDataToVariant |> Js.Promise.resolve
         ),
    fetchMore: (~variables=?, ~updateQuery, ()) =>
      apolloData->(
                    fetchMoreGet(
                      fetchMoreOptions(~variables?, ~updateQuery, ()),
                    )
                  ),
    networkStatus: apolloData->networkStatusGet,
    subscribeToMore:
      (~document, ~variables=?, ~updateQuery=?, ~onError=?, ()) =>
      apolloData->(
                    subscribeToMoreGet(
                      subscribeToMoreOptions(
                        ~document,
                        ~variables?,
                        ~updateQuery?,
                        ~onError?,
                        (),
                      ),
                    )
                  ),
  };

  let make =
      (
        ~client: option(generatedApolloClient)=?,
        ~variables: option(Js.Json.t)=?,
        ~pollInterval: option(int)=?,
        ~notifyOnNetworkStatusChange: option(bool)=?,
        ~fetchPolicy: option(string)=?,
        ~errorPolicy: option(string)=?,
        ~ssr: option(bool)=?,
        ~displayName: option(string)=?,
        ~delay: option(bool)=?,
        ~context: option(Js.Json.t)=?,
        children: renderPropObj => ReasonReact.reactElement,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=queryComponent,
      ~props=
        Js.Nullable.{
          "query": graphqlQueryAST,
          "client": client |> fromOption,
          "variables": variables |> fromOption,
          "pollInterval": pollInterval |> fromOption,
          "notifyOnNetworkStatusChange":
            notifyOnNetworkStatusChange |> fromOption,
          "fetchPolicy": fetchPolicy |> fromOption,
          "errorPolicy": errorPolicy |> fromOption,
          "ssr": ssr |> fromOption,
          "displayName": displayName |> fromOption,
          "delay": delay |> fromOption,
          "context": context |> fromOption,
        },
      apolloData =>
      apolloData |> convertJsInputToReason |> children
    );
};

/*
 * Expose a module to perform "mutation" operations for the given client
 */
module CreateMutation = (Config: Config) => {
  external cast:
    string =>
    {
      .
      "data": Js.Json.t,
      "loading": bool,
    } =
    "%identity";
  [@bs.module "react-apollo"]
  external mutationComponent: ReasonReact.reactClass = "Mutation";
  let graphqlMutationAST = gql(. Config.query);
  type response = mutationResponse(Config.t);
  type renderPropObj = {
    result: response,
    data: option(Config.t),
    loading: bool,
    error: option(apolloError),
    networkStatus: int,
  };
  type apolloMutation =
    (~variables: Js.Json.t=?, ~refetchQueries: array(string)=?, unit) =>
    Js.Promise.t(mutationRenderPropObjJS);
  [@bs.obj]
  external makeMutateParams:
    (~variables: Js.Json.t=?, ~refetchQueries: array(string)=?) => _ =
    "";
  let apolloMutationFactory =
      (~jsMutation, ~variables=?, ~refetchQueries=?, ()) =>
    jsMutation(makeMutateParams(~variables?, ~refetchQueries?));
  let apolloDataToReason: mutationRenderPropObjJS => response =
    apolloData =>
      switch (
        apolloData##loading,
        apolloData##data |> Utilities.getNonEmptyObj,
        apolloData##error |> Js.Nullable.toOption,
      ) {
      | (true, _, _) => Loading
      | (false, Some(data), _) => Data(Config.parse(data))
      | (false, _, Some(error)) => Error(error)
      | (false, None, None) => NotCalled
      };
  let convertJsInputToReason = (apolloData: mutationRenderPropObjJS) => {
    result: apolloDataToReason(apolloData),
    data:
      switch (apolloData##data |> Utilities.getNonEmptyObj) {
      | None => None
      | Some(data) =>
        switch (Config.parse(data)) {
        | parsedData => Some(parsedData)
        | exception _ => None
        }
      },
    error: apolloData##error |> Js.Nullable.toOption,
    loading: apolloData##loading,
    networkStatus: apolloData##networkStatus,
  };
  let make =
      (
        ~client: option(generatedApolloClient)=?,
        ~variables: option(Js.Json.t)=?,
        ~onError: option(unit => unit)=?,
        ~onCompleted: option(unit => unit)=?,
        children: (apolloMutation, renderPropObj) => ReasonReact.reactElement,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=mutationComponent,
      ~props=
        Js.Nullable.{
          "mutation": graphqlMutationAST,
          "client": client |> fromOption,
          "variables": variables |> fromOption,
          "onError": onError |> fromOption,
          "onCompleted": onCompleted |> fromOption,
        },
      (mutation, apolloData) =>
      children(
        apolloMutationFactory(~jsMutation=mutation),
        convertJsInputToReason(apolloData),
      )
    );
};

/*
 * Expose a module to perform "subscription" operations for the given client
 */
module CreateSubscription = (Config: Config) => {
  [@bs.module "react-apollo"]
  external subscriptionComponent: ReasonReact.reactClass = "Subscription";

  let graphQLSubscriptionAST = gql(. Config.query);

  type response = subscriptionResponse(Config.t);

  type renderPropObj = {
    result: response,
    data: option(Config.t),
    error: option(apolloError),
    loading: bool,
  };

  type renderPropObjJS = {
    .
    "loading": bool,
    "data": Js.Nullable.t(Js.Json.t),
    "error": Js.Nullable.t(apolloError),
  };

  let apolloDataToVariant: renderPropObjJS => response =
    apolloData =>
      switch (
        apolloData##loading,
        apolloData##data |> Js.Nullable.toOption,
        apolloData##error |> Js.Nullable.toOption,
      ) {
      | (true, _, _) => Loading
      | (false, Some(response), _) => Data(Config.parse(response))
      | (false, _, Some(error)) => Error(error)
      | (false, None, None) =>
        Error({
          "message": "No data",
          "graphQLErrors": Js.Nullable.null,
          "networkError": Js.Nullable.null,
        })
      };

  let convertJsInputToReason: renderPropObjJS => renderPropObj =
    apolloData => {
      result: apolloData |> apolloDataToVariant,
      data:
        switch (apolloData##data |> Utilities.getNonEmptyObj) {
        | None => None
        | Some(data) =>
          switch (Config.parse(data)) {
          | parsedData => Some(parsedData)
          | exception _ => None
          }
        },
      error:
        switch (apolloData##error |> Js.Nullable.toOption) {
        | Some(error) => Some(error)
        | None => None
        },
      loading: apolloData##loading,
    };

  let make =
      (
        ~variables: option(Js.Json.t)=?,
        ~children: renderPropObj => ReasonReact.reactElement,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=subscriptionComponent,
      ~props=
        Js.Nullable.{
          "subscription": graphQLSubscriptionAST,
          "variables": variables |> fromOption,
        },
      apolloData =>
      apolloData |> convertJsInputToReason |> children
    );
};
