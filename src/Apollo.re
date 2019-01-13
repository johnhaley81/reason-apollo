include Apollo_Types;

/**
 * The signature of the `graphql-tag/gql` function that transforms a GraphQL
 * query string to the standard GraphQL AST.
 * https://github.com/apollographql/graphql-tag
 */
type gql = (. string) => queryString;

[@bs.module] external gql: gql = "graphql-tag";

module type Config = {
  let query: string;
  type t;
  let parse: Js.Json.t => t;
};

module type ResultConfig = {
  let query: string;
  type t;
  let parse: Js.Json.t => Belt.Result.t(t, Decode.ParseError.failure);
};

module Error = {
  type t = {
    message: string,
    graphqlErrors: option(array(string)),
    networkError: option(string),
  };

  let make = (message, graphqlErrors, networkError) => {
    message,
    graphqlErrors,
    networkError,
    /* TODO: extraInfo */
  };

  let parse = json =>
    Decode.AsOption.(
      Pipeline.succeed(make)
      |> Pipeline.field("message", string)
      |> Pipeline.optionalField("graphQLErrors", array(string))
      |> Pipeline.optionalField("networkError", field("message", string))
      |> Pipeline.run(json)
    );
};

type apolloError = {
  .
  "message": string,
  "graphQLErrors": Js.Nullable.t(array(string)),
  "networkError": Js.Nullable.t(string),
};

type documentNodeT;

type splitTest = {. "query": documentNodeT};

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

module QueryResponse = {
  [@bs.deriving abstract]
  type t = {
    loading: bool,
    data: Js.Json.t,
    error: Js.Json.t,
    networkStatus: Js.Nullable.t(int),
    variables: Js.Json.t,
  };

  [@bs.send]
  external refetch: (t, Js.Nullable.t(Js.Json.t)) => Js.Promise.t(t) = "";
  let refetch = (response, json) =>
    refetch(response, Js.Nullable.fromOption(json));

  [@bs.send]
  external fetchMore: (t, fetchMoreOptions) => Js.Promise.t(unit) = "";

  [@bs.send]
  external subscribeToMore: (t, subscribeToMoreOptions, unit) => unit = "";
};

type mutationRenderPropObjJS = {
  .
  "loading": bool,
  "called": bool,
  "data": Js.Nullable.t(Js.Json.t),
  "error": Js.Nullable.t(apolloError),
  "networkStatus": int,
  "variables": Js.Nullable.t(Js.Json.t),
};

type client;

type context;
type info;

type resolverFunc = (Js.Json.t, Js.Json.t, context, info) => Js.Json.t;
type resolver = {
  field: string,
  resolver: resolverFunc,
};

type initFunc = client => Js.Json.t;
type init = {
  field: string,
  init: initFunc,
};

module Context = {
  [@bs.get] external cacheGet: context => apolloCache = "cache";
};

module Info = {};

module Initializers = {
  type t = Js.Dict.t(initFunc);

  let recordToTuple = ({field, init}) => (field, init);

  /* TODO: inteface? */
  let make = inits => Belt.Array.map(inits, recordToTuple)->Js.Dict.fromArray;
};

module Resolvers = {
  type t = Js.Dict.t(Js.Dict.t(resolverFunc));

  let recordToTuple = ({field, resolver}) => (field, resolver);

  /* TODO: interfaces */
  let keyed = (key, resolvers) =>
    Js.Dict.fromArray([|
      (key, Belt.Array.map(resolvers, recordToTuple)->Js.Dict.fromArray),
    |]);
  let queries = resolvers => keyed("Query", resolvers);
  let mutations = resolvers => keyed("Mutation", resolvers);
};

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

module Utilities = {
  type operationDefinitionNode = {
    .
    "kind": string,
    "operation": string,
  };

  [@bs.module "apollo-utilities"]
  external getMainDefinition: documentNodeT => operationDefinitionNode =
    "getMainDefinition";
};

module Client = {
  [@bs.module "apollo-client"] [@bs.new]
  external createApolloClientJS: 'a => client = "ApolloClient";

  [@bs.deriving abstract]
  type mutateOptions = {
    mutation: queryString,
    [@bs.optional]
    variables: Js.Json.t,
  };

  [@bs.send]
  external mutate_:
    (client, mutateOptions) => Js.Promise.t(mutationRenderPropObjJS) =
    "mutate";
  let mutate = (~mutation, ~variables=?, client) =>
    mutate_(client, mutateOptions(~mutation, ~variables?, ()));

  [@bs.deriving abstract]
  type queryOptions = {
    query: queryString,
    [@bs.optional]
    variables: Js.Json.t,
  };

  [@bs.send]
  external query_: (client, queryOptions) => Js.Promise.t(QueryResponse.t) =
    "query";
  let query = (~query, ~variables=?, client) =>
    query_(client, queryOptions(~query, ~variables?, ()));

  [@bs.send] external resetStore: client => unit = "";

  [@bs.deriving abstract]
  type apolloClientObjectParam = {
    link: apolloLink,
    cache: apolloCache,
    [@bs.optional]
    ssrMode: bool,
    [@bs.optional]
    ssrForceFetchDelay: int,
    [@bs.optional]
    connectToDevTools: bool,
    [@bs.optional]
    queryDeduplication: bool,
    [@bs.optional]
    initializers: array(Initializers.t),
    [@bs.optional]
    resolvers: array(Resolvers.t),
    [@bs.optional]
    typeDefs: string,
  };

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
        ~initializers=?,
        ~resolvers=?,
        ~typeDefs=?,
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
        ~initializers?,
        ~resolvers?,
        ~typeDefs?,
        (),
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

  module Error = Apollo_Link_Error;
  module Context = Apollo_Link_Context;
  module Http = Apollo_Link_Http;
  module Upload = Apollo_Link_Upload;
  module WebSocket = Apollo_Link_WebSocket;
};

module Cache = {
  module InMemory = Apollo_Cache_InMemory;
  module Persist = Apollo_Cache_Persist;

  [@bs.deriving abstract]
  type writeQueryOptions = {
    query: queryString,
    data: Js.Json.t,
  };

  [@bs.deriving abstract]
  type readQueryOptions = {query: queryString};

  [@bs.send]
  external writeQuery_: (apolloCache, writeQueryOptions) => unit =
    "writeQuery";
  [@bs.send]
  external readQuery_: (apolloCache, readQueryOptions) => Js.Json.t =
    "readQuery";

  let writeQuery = (~query, ~data, cache) =>
    writeQuery_(cache, writeQueryOptions(~query, ~data));

  let readQuery = (~query, cache) =>
    readQuery_(cache, readQueryOptions(~query));
};

module Provider = {
  [@bs.module "react-apollo"]
  external apolloProvider: ReasonReact.reactClass = "ApolloProvider";

  let make = (~client: client, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=apolloProvider,
      ~props={"client": client},
      children,
    );
};

module Consumer = {
  [@bs.module "react-apollo"]
  external apolloConsumer: ReasonReact.reactClass = "ApolloConsumer";

  let make = (children: client => ReasonReact.reactElement) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=apolloConsumer,
      ~props=Js.Obj.empty(),
      children,
    );
};

/*
 * Expose a module to perform "query" operations for the given client
 */
module CreateQuery = (Config: ResultConfig) => {
  [@bs.module "react-apollo"]
  external component: ReasonReact.reactClass = "Query";

  type response =
    | Loading
    | Error(Error.t)
    | ParseError(Decode.ParseError.failure)
    | Data(Config.t);

  type renderPropObj = {
    loading: bool,
    result: response,
    data: Belt.Result.t(Config.t, Decode.ParseError.failure),
    error: option(Error.t),
    refetch: option(Js.Json.t) => Js.Promise.t(response),
    fetchMore:
      (~variables: Js.Json.t=?, ~updateQuery: updateQueryT, unit) =>
      Js.Promise.t(unit),
    networkStatus: option(int),
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
  let toVariant = (loading, data, error) =>
    switch (loading, data, error) {
    | (true, _, _) => Loading
    | (false, Belt.Result.Error(error), None) => ParseError(error)
    | (false, Belt.Result.Ok(response), None) => Data(response)
    | (false, _, Some(error)) => Error(error)
    };

  let getData = response => {
    let loading = response->QueryResponse.loadingGet;
    let data = response->QueryResponse.dataGet->Config.parse;
    let error = response->QueryResponse.errorGet->Error.parse;
    (loading, data, error);
  };

  let convertJsInputToReason = response => {
    let (loading, data, error) = getData(response);
    let result = toVariant(loading, data, error);

    {
      result,
      data,
      error,
      loading,
      refetch: variables =>
        response->QueryResponse.refetch(variables)
        |> Js.Promise.then_(response => {
             let (loading, data, error) = getData(response);
             toVariant(loading, data, error) |> Js.Promise.resolve;
           }),
      fetchMore: (~variables=?, ~updateQuery, ()) =>
        response->QueryResponse.fetchMore(
          fetchMoreOptions(~variables?, ~updateQuery, ()),
        ),
      networkStatus:
        response->QueryResponse.networkStatusGet->Js.Nullable.toOption,
      subscribeToMore:
        (~document, ~variables=?, ~updateQuery=?, ~onError=?, ()) =>
        response->QueryResponse.subscribeToMore(
          subscribeToMoreOptions(
            ~document,
            ~variables?,
            ~updateQuery?,
            ~onError?,
            (),
          ),
        ),
    };
  };

  let make =
      (
        ~client: option(client)=?,
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
      ~reactClass=component,
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
      apolloData->convertJsInputToReason->children
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
  type response =
    | Loading
    | Error(apolloError)
    | Data(Config.t)
    | NotCalled;
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
        apolloData##data |> getNonEmptyObj,
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
      switch (apolloData##data |> getNonEmptyObj) {
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
        ~client: option(client)=?,
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

  type response =
    | Loading
    | Error(apolloError)
    | Data(Config.t);

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
        switch (apolloData##data |> getNonEmptyObj) {
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
        ~client: option(client)=?,
        ~variables: option(Js.Json.t)=?,
        ~children: renderPropObj => ReasonReact.reactElement,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=subscriptionComponent,
      ~props=
        Js.Nullable.{
          "subscription": graphQLSubscriptionAST,
          "client": client |> fromOption,
          "variables": variables |> fromOption,
        },
      apolloData =>
      apolloData |> convertJsInputToReason |> children
    );
};
