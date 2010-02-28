require 'test/unit'
require 'rbclips'

class Test_Constraint < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Constraint)
    assert Clips::Constraint.constants.member?(:Creator)
  end

  def test_new_hash
    # :type checking
    assert_nothing_raised               { Clips::Constraint.new :type => :any }
    assert_nothing_raised               { Clips::Constraint.new :type => [:symbol, :string, :lexeme, :integer, :float, :number, :instance_name, :instance_address, :instance, :external_address, :fact_address] }
    assert_nothing_raised               { Clips::Constraint.new :type => ['symbol', 'string', 'lexeme', 'integer', 'float', 'number', 'instance_name', 'instance_address', 'instance', 'external_address', 'fact_address'] }
    assert_nothing_raised               { Clips::Constraint.new :type => ['symbol', 'string', :lexeme, 'integer', 'float', 'number', :instance_name, 'instance_address', 'instance', :external_address, 'fact_address'] }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :type => [:any] }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :type => [:free_sex_for_everyone] }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :type => :microsoft_to_paraler_universe }

    # :values checking
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :values => :free_beer_for_everyone }
    assert_nothing_raised               { Clips::Constraint.new :values => [:i, "hate", "the", "firewall", "guy"] }

    # :range checking
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :range => "hate microsoft" }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :range => nil }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :range => true }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :range => false }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :range => 2 }
    assert_nothing_raised               { Clips::Constraint.new :range => 3..12 }

    # :cardinality checking
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :cardinality => "hate microsoft" }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :cardinality => nil }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :cardinality => true }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :cardinality => false }
    assert_raise(Clips::ArgumentError)  { Clips::Constraint.new :cardinality => 2 }
    assert_nothing_raised               { Clips::Constraint.new :cardinality => 3..12 }

    a = Clips::Constraint.new :type => :any
    assert_equal a.instance_eval { @type }, [:any]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new :type => [:string, :lexeme]
    assert_equal a.instance_eval { @type }, [:string, :lexeme]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new :type => :any, :range => 3..13
    assert_equal a.instance_eval { @type }, [:any]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, 3..13
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new  :cardinality => 3..18
    assert_equal a.instance_eval { @type }, nil
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, 3..18
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Constraint::Creator.new }
    assert_nothing_raised               { Clips::Constraint.new {|c| c.type :any } }
    assert_nothing_raised               { Clips::Constraint.new {|c| c.values [:any] } }
    assert_nothing_raised               { Clips::Constraint.new {|c| c.range 3..13 } }
    assert_nothing_raised               { Clips::Constraint.new {|c| c.cardinality 3..13 } }

    a = Clips::Constraint.new {|c| c.type :any }
    assert_equal a.instance_eval { @type }, [:any]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new {|c| c.type [:string, :lexeme] }
    assert_equal a.instance_eval { @type }, [:string, :lexeme]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new {|c| c.type :any; c.range 3..13 }
    assert_equal a.instance_eval { @type }, [:any]
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, 3..13
    assert_equal a.instance_eval { @cardinality }, nil

    a = Clips::Constraint.new {|c| c.cardinality 3..18 }
    assert_equal a.instance_eval { @type }, nil
    assert_equal a.instance_eval { @values }, nil
    assert_equal a.instance_eval { @range }, nil
    assert_equal a.instance_eval { @cardinality }, 3..18
  end

  def test_to_s
    assert_equal Clips::Constraint.new(:type => :any).to_s, "(type ?VARIABLE ) "
    assert_equal Clips::Constraint.new(:type => [:symbol, :string]).to_s, "(type SYMBOL STRING ) "
    assert_equal Clips::Constraint.new(:type => %w(symbol string)).to_s, "(type SYMBOL STRING ) "
    assert_equal Clips::Constraint.new(:type => %w(symbol string), :values => %w(ahoj nazdar)).to_s, "(type SYMBOL STRING ) (values \"ahoj\" \"nazdar\" ) "
    assert_equal Clips::Constraint.new(:values => %w(ahoj nazdar), :range => 3..13).to_s, "(values \"ahoj\" \"nazdar\" ) (range 3 13) "
    assert_equal Clips::Constraint.new(:values => [:symbol, "text"]).to_s, "(values symbol \"text\" ) "
    assert_equal Clips::Constraint.new(:values => %w(ahoj nazdar), :cardinality => 3..13).to_s, "(values \"ahoj\" \"nazdar\" ) (cardinality 3 13) "
    assert_equal Clips::Constraint.new(:range => 4..14, :cardinality => 3..13).to_s, "(range 4 14) (cardinality 3 13) "
  end

  def test_equal
    t = Proc.new do |hash| 
      a = Clips::Constraint.new hash
      b = Clips::Constraint.new hash
      assert a == b
      assert a.eql? b
      assert a.equal? b

      c = a.dup
      assert c == a
      assert c == b
    end

    t.call :type => :any
    t.call :type => :integer
    t.call :type => %w(symbol string)
    t.call :values => %w(ahoj nazdar hello hi)
    t.call :range => 3..13
    t.call :cardinality => 4..14
  end
end
